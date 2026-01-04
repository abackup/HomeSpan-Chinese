<!-- 原文时间：2026.1.1，校对时间：2026.1.4 -->

# 可寻址 RGB LED
HomeSpan 包含三个专用类，可轻松控制“可寻址” RGB LED：

* **Pixel()** - 此用于仅需一根“数据”控制线的 RGB、RGBW、RGBWC 和 WC LED，这些 LED 使用 NeoPixel 协议，例如这款 8 像素  [NeoPixel RGB Stick](https://www.adafruit.com/product/1426) 或这个单像素的 [NeoPixel RGBW LED](https://www.adafruit.com/product/2759)。

* **Dot()** - 此类用于使用 DotStar 协议的需要两条控制线（“数据”和“时钟”）的 RGB LED，例如这款 144 像素[DotStar RGB Strip](https://www.adafruit.com/product/2241) 或这个 60 像素的 [RGB LED Strip](https://www.sparkfun.com/products/14015) 。

* **WS2801_LED()**- 此用于需要两根控制线（“数据”和“时钟”）的 RGB LED，这些控制线使用 WS2801 协议，例如这款 25 像素[漫射平面数字 RGB LED 灯串](https://www.adafruit.com/product/738)。

这三个类都允许您将多像素 LED 灯条中的每个“像素”单独设置为不同的 24 位 RGB（红/绿/蓝）颜色。Pixel 类还支持 16 位 WC（暖白/冷白）像素、32 位 RGBW（红/绿/蓝/白）像素和 40 位 RGBWC（即红/绿/蓝/暖白/冷白）像素。此外，这些类还允许您简单地指定一种颜色，并将其复制到所有像素。

每个类的方法几乎完全相同，这使得您只需稍作修改，即可轻松地将为一种设备编写的代码与另一种设备的代码互换使用。

## *Pixel(uint8_t pin, [const char \*pixelType])*

创建此**类**的实例将配置指定的**引脚**以输出波形信号，该信号适用于控制具有任意像素数的单线可寻址 RGB、WC、RGBW 或 RGBWC LED 设备。参数及其默认值（如果未指定）如下所示：

  * *pin* - 将输出 RGB 控制信号的引脚;通常连接到可寻址 LED 设备的“数据”输入
    
  * *pixelType* - 一个字符串，指定要控制的 LED 类型（例如 WC 或 RBGW）以及颜色数据的传输顺序（例如 RGB 与 GBR）。
    * 对于 *3 色*设备（例如 RGB LED），应按照设备[^1]所需的任何顺序将 *pixelType* 设置为字符 "RGB"
    * 对于 *4 色*设备（例如 RGB LED 加白色 LED），应按照设备要求的任何顺序将 *pixelType* 设置为字符“RGBW”
    * 对于 *5 色*设备（例如 RGB LED 加上单独的暖白光和冷白光 LED），应按照设备要求的任何顺序将 *pixelType* 设置为字符“RGBWC”
    * 对于 *2 色* 设备（例如，仅限暖白色和冷白色 LED），*pixelType* 应设置为字符“WC-”，顺序由设备决定，其中“-”表示虚拟字节，因为暖白色/冷白色 LED 灯带制造商通常使用 3 通道设备驱动程序（设计用于操作 RGB LED），但仅将 2 个通道（例如红色和绿色）连接到暖白色和冷白色 LED，而第三个通道（在此示例中为蓝色）未连接。
  * 示例：`Pixel myDevice(26, "WBRG");` 创建一个连接到引脚 26 的 4 色 RGBW 设备，其中颜色按白色、蓝色、红色、绿色的顺序传输
  * 请注意，*pixelType* 是可选的。如果未指定，则默认值为“GRB”

[^1]:一些 NeoPixel 设备只有固定温度的白色 LED，连接到单个像素的常见红色、绿色和蓝色输出（例如，参见此 [Adafruit 3000K NeoPixel 设备](https://www.adafruit.com/product/2376)）。如果您希望单独控制与单个像素相关的每个白色 LED，请将 *pixelType* 设置为“RGB”，并假装您正在操作 RGB NeoPixel 设备。或者，您可以将 *pixelType* 设置为“WWW”，在这种情况下，**Pixel** 库将同时控制单个像素内的所有三个白色 LED。

> [!TIP]
> 由于您的特定设备可能具有哪种类型的 LED 通常并不明显，因此 HomeSpan 包含一个草图，旨在帮助您确定 *pixelType* 的正确值。有关详细说明，请参阅 Arduino IDE 下的 [*文件 → 示例 → HomeSpan → 其他示例 → PixelTester*](../examples/Other%20Examples/PixelTester)。如果您发现 Pixel 设备的颜色与“家庭”应用中显示的颜色不匹配，请使用此测试器草图。

设置像素颜色的两种主要方法是：

* `void set(Color color, int nPixels=1)`

  * 将单像素设备中像素的颜色设置为 *color*，或者将多像素设备中前 *nPixels* 的颜色设置为 *color*，其中 *color* 是下面定义的 **Color** 类型的对象。如果未指定，*nPixels* 默认为 1（即单个像素）。如果为 *nPixels* 指定的值与设备中实际 RGB 像素的总数不匹配，则没有问题；如果 *nPixels* 小于设备像素的总数，则只有前 *nPixels* 会设置为 *color*；如果 *nPixels* 大于设备像素的总数，则设备将忽略额外的输入
  
* `void set(Color *color, int nPixels)`

  * 将多像素设备中每个像素的颜色分别设置为 **Color** 数组 *\*color* 中指定的颜色值，大小为 *nPixels*，其中设备的第一个像素设置为 *color\[0\]* 中的值，第二个像素设置为 *color\[1\]* 中的值……最后一个像素设置为 *color\[nPixels-1\]* 中的值。与上述类似，如果为 *nPixels* 指定的值与设备中实际 RGB 像素总数不匹配，则不是问题

请注意，灯带上的 LED 数量可能多于可控制像素的总数。对于 5 伏灯带，每个驱动芯片通常只有一个 LED。对于 12 伏和 24 伏灯带，制造商通常会将多个 LED 连接到每个像素驱动芯片。例如，300 个 LED 灯带可能包含 100 个像素驱动芯片，其中每个芯片控制三个 LED（设置为相同颜色）。始终将 *nPixels* 设置为灯带中的像素驱动芯片数量，而不是 LED 数量

在上述两种方法中，Pixel 类将颜色本机存储为 40 位 **Color** 对象，该对象配置为按 RGBWC 顺序保存五个 8 位值（无论 *pixelType* 是否设置为“RGB”、“WGRB”、“W-C”等）。 **Color** 对象可以实例化为单个变量（例如 `Pixel::Color myColor;`）或数组（例如 `Pixel::Color myColors[8];`）。请注意，**Pixel** 类使用的 **Color** 对象的作用域是 **Pixel** 类本身，因此您需要使用完全限定的类名“Pixel::Color”。
创建 **Color** 对象后，可以使用以下方法之一设置其存储的颜色：
  
* `Color RGB(uint8_t r, uint8_t g, uint8_t b, uint8_t w=0, uint8_t c=0)`

  * 其中 *r*、*g* 和 *b* 表示 0-255 范围内的 8 位红色、绿色和蓝色值，*w* 表示可选的暖白色 8 位值，*c* 表示可选的冷白色 8 位值。白色值可以不指定，在这种情况下默认为 0
  * 返回 **Color** 对象
  * 示例：`myColor.RGB(255,255,0)` 将 myColor 设置为亮黄色
  * 示例：`myColor.RGB(255,0,0,128,128)` 将 myColor 设置为最大强度红色与半强度暖白色和冷白色的组合
      
* `Color HSV(float h, float s, float v, double w=0, double c=0)`
    
  * 其中 *h*=色调，范围为 0-360；*s*=饱和度百分比，范围为 0-100；*v*=亮度百分比，范围为 0-100。这些值将转换为等效的 8 位 RGB 值（0-255），以存储在 **Color** 对象中。请注意，*w* 和 *c* 值是分开处理的，表示暖白色和冷白色 LED 的可选亮度百分比（0-100）。与上述类似，暖白色和冷白色值可以不指定，在这种情况下，它们的默认值为 0
  * 返回 **Color** 对象
  * 示例：`myColor.HSV(120,100,50)` 将 myColor 设置为亮度为 50% 的完全饱和绿色

* `Color WC(uint8_t w, uint8_t c=0)`
    
  * 将 **Color** 对象的暖白色和冷白色部分分别设置为 *w* 和 *c*，其中值的范围为 0-255
  * 冷白色值可以不指定，在这种情况下默认为 0
  * 此方法与调用 `RGB(0,0,0,w,c);` 相同
  * 返回一个 **Color** 对象
   
* `Color CCT(float temp, float v, float wTemp, float cTemp)`

  * 根据用户指定的暖白色 (*wTemp*) 和冷白色 (*cTemp*) LED 值（以开尔文为单位），将 **Color** 对象的暖白色和冷白色部分设置为相关色温 *temp*（以开尔文为单位），亮度百分比 *v*（0-100）
  * 如果指定的 *temp* 超出暖白色和冷白色温度范围，则将自动重置为匹配暖白色或冷白色温度，以防止出现越界错误
  * 返回 **Color** 对象
  * 示例：`myColor.CCT(4000,100,3000,6500)` 通过插入用户指定的暖白色 (3000K) 和冷白色 (6500K) LED 温度，将 myColor 设置为 CCT 等效值 4000K，亮度为 100%。请注意，此计算的结果相当于 `myColor.WC(183,72)`
  * 示例：`myColor.CCT(4000,25,3000,6500)` 通过插入用户指定的暖白 (3000K) 和冷白 (6500K) LED 温度，然后将这些值按 25% 的倍数缩放，将 myColor 设置为 CCT 等效值 4000K，亮度为 25%。此计算的结果相当于 `myColor.WC(45,18)`
    
请注意，上述两种方法都返回完整的**Color**对象本身，因此可以在任何需要**Color**对象的地方使用：例如：`Pixel p(5); Pixel::Color myColor; p.set(myColor.RGB(255,215,0))`将连接到引脚 5 的单个像素设备的颜色设置为亮金色。

**Pixel** 类还支持以下*类级*方法，作为创建颜色的便捷替代方法：
  
* `static Color RGB(uint8_t r, uint8_t g, uint8_t b, uint8_t w=0, uint8_t c=0)`
  * 相当于 `return(Color().RGB(r,g,b,w,c));`
  * 返回生成的新 **Color** 对象
  * 示例：`Pixel p(5,8); p.set(Pixel::RGB(0,0,255),8);` 将 8 像素设备（连接到引脚 5）中每个像素的颜色设置为蓝色

* `static Color HSV(float h, float s, float v, double w=0, double c=0)`
  * 相当于 `return(Color().HSV(h,s,v,w,c));`
  * 返回生成的新 **Color** 对象
  * 示例：`Pixel::Color c[]={Pixel::HSV(120,100,100),Pixel::HSV(60,100,100),Pixel::HSV(0,100,100)};` 创建绿黄红交通灯图案

* `static Color WC(uint8_t w, uint8_t c=0)`
  * 相当于 `return(Color().WC(w,c));`
  * 返回生成的新 **Color** 对象
  * 冷白色值可以不指定，在这种情况下默认为 0
  * 示例：`Pixel p(5); p.set(Pixel::WC(0,128));` 将单个像素设备（连接到引脚 5）的颜色设置为半强度冷白色
 
* `static Color CCT(float temp, float v, float wTemp, float cTemp)`
  * 相当于 `return(Color().CCT(temp,v,wTemp,cTemp));`
  * 如果指定的 *temp* 超出暖白和冷白温度范围，它将自动重置为匹配暖白或冷白温度，以防止出现越界错误
  * 返回生成的新 **Color** 对象
  * 示例：`Pixel p(5); p.set(Pixel::CCT(4000,100,3000,6500),8);` 将 8 像素设备（连接到引脚 5）中每个像素的颜色设置为全强度 4000K 白色，假设设备上暖白和冷白 LED 的色温分别为 3000K 和 6500K。

为了避免每次创建 CCT 颜色时都必须重复指定任何给定灯带的暖白色和冷白色 LED 的色温，**Pixel** 类包含两种 *成员级* 方法，允许您仅对任何给定设备指定一次色温，然后使用它们为 *该特定设备* 设置任何 CCT 颜色：

* `Pixel *setTemperatures(float wTemp, float cTemp)`
  * 指定特定灯带的暖白色 LED (*wTemp*) 和冷白色 LED (*cTemp) 温度（以开尔文为单位）
  * 返回指向 **Pixel** 对象的指针

* `Color CCT(float temp, float v)`
  * 根据之前使用上面的 `setTemperatures(float wTemp, float cTemp)` 为设备指定的暖白色和冷白色 LED 温度，为特定像素设备创建温度为 *temp*（以开尔文为单位）且亮度百分比为 *v*（从 0-100）的 CCT 颜色
  * 如果您尚未为此设备调用 `setTemperatures()`，则暖白色使用默认值 2000K，冷白色使用默认值 7000K
  * 实际上相当于使用 *类级* 方法，如下所示：`return(Color().CCT(temp,v,warmTemp,coolTemp));`
  * 如果指定的 *temp* 超出暖白色和冷白色温度范围，它将自动重置为匹配暖白色或冷白色温度，以防止出现越界错误
  * 返回生成的新 **Color** 对象
 
**Pixel** 类还支持以下*成员级*方法：
 
* `int getPin()`

  * 返回 pin 号，如果实例化由于缺乏资源而失败，则返回 -1(see below) or for any other reason

* `boolean hasColor(char c)`
  
  * 如果 *pixelType* 包含 *c*（不区分大小写），则返回 true，否则返回 false
  * *c* 应为以下之一：'R'、'G'、'B'、'W'、'C' 或它们的小写等效项
    
* `Pixel *setTiming(float high0, float low0, float high1, float low1, uint32_t lowReset)`

  * **Pixel** 类用于生成设置全彩 LED 设备颜色所需的“数据”信号的默认时序参数应该适用于大多数基于 SK6812 或 WS2812 驱动器芯片的商业产品。如果你需要重写类默认值并将其替换为你自己的计时参数，请使用此方法**仅有的**，其中
    * *high0* 和 *low0* 指定编码零位的脉冲的高相位和低相位的持续时间（以微秒为单位）；
    * *high1* 和 *low1* 指定编码一位的脉冲的高相位和低相位的持续时间（以微秒为单位）；和
    * *lowReset* 指定表示脉冲流结束的延迟（以微秒为单位）
  * 作为参考，**Pixel** 类使用以下默认值：*high0 为 0.32𝛍s, low0 为 0.88𝛍s, high1 为 0.64𝛍s, low1 为 0.56𝛍s, lowReset 为 80.0𝛍s* 
  * 返回指向 **Pixel** 对象的指针

### 资源使用情况

 **Pixel** 类依赖 ESP32 的 RMT 外设来创建控制单线可寻址全彩 LED 所需的精确脉冲序列。由于 **Pixel** 的每个实例化都会消耗一个 RMT 通道，因此你可以实例化的 **Pixel** 对象的数量（每个对象控制连接到特定引脚的单独的多像素全彩 LED 设备）仅限于可用的 RMT 通道数量，如下所示：

* ESP32 - 8 通道;
* ESP32-S2 - 4 通道;
* ESP32-S3 - 4 通道;
* ESP32-C3 - 2 通道;
* ESP32-C5 - 2 通道;
* ESP32-C6 - 2 通道;

请注意，**RFControl** 类也使用 ESP32 的 RMT 外围设备，因此 **RFControl** 的任何实例也将消耗 RMT 通道（例如，在 ESP32-C3 上，您可以创建两个像素、两个 RFControls 或一个像素和一个 RFControl）。
 
### 故障排除提示

* 如果灯带似乎可以正常工作，但颜色不正确，请检查您是否已正确设置 *pixelType*，方法是将灯带上的所有 LED 颜色设置为红色

  * 如果所有 LED 都显示 *相同* 但不正确的颜色（即绿色而不是红色），则需要更改 *pixelType* 中颜色字符的顺序以进行相应匹配
  * 如果每个 LED 显示不同的颜色（或某些 LED 完全不亮），则您可能将 *pixelType* 错误地指定为错误的颜色数量。例如，您指定了“RGBW”，但您的设备仅支持“RGB”，反之亦然
* 如果灯带闪烁或快速打开后再关闭，则您的电源可能尺寸太小，无法为灯带中的所有 LED 提供足够的电流
  * 尝试降低亮度 - 如果 LED 稳定下来并且仅在特定亮度水平以上闪烁，则您的电源尺寸太小
  * 尝试设置纯色（即仅红色或仅暖白色）。如果 LED 稳定下来，并且仅在您同时设置多种 LED 颜色时闪烁，则您的电源尺寸太小
  * 尝试仅设置灯串上的前几个像素。如果您有 100 像素的灯串，并且仅在您尝试设置超过 60 个像素时 LED 才闪烁，则您的电源尺寸太小
* 如果灯带行为不稳定或根本不工作
  * 这可能是由于时序参数不匹配造成的。要解决此问题，请在网上搜索特定设备使用的确切 **芯片** 的规格文档。这些文档通常提供一个表格，列出传输二进制 1 或二进制 0 所需的 HIGH 和 LOW 周期的持续时间。然后，您可以使用上面的 `setTiming()` 方法相应地设置时序参数
  * 这可能是由于使用了错误的电源电压 - 检查以确保您没有在需要 12V 或 24V 的灯带上使用 5V 电源 
  
## *Dot(uint8_t dataPin, uint8_t clockPin)*

创建此**类**实例会将指定的引脚配置为输出适用于控制基于 DotStar 协议的双线可寻址 RGB LED 设备的波形信号，该设备可具有任意数量的像素。参数及其默认值（如果未指定）如下：
  * *dataPin* - 将输出 RGB 数据信号的引脚；通常连接到可寻址 LED 设备的“数据”输入
  * *clockPin* - 将输出 RGB 时钟信号的引脚；通常连接到可寻址 LED 器件的“时钟”输入

设置像素颜色的两种主要方法是：

* `void set(Color color, int nPixels=1)`

  * 将单像素设备中像素的颜色设置为 *color*，或者等效地将多像素设备中第一个 *nPixels* 的颜色设置为 *color*，其中 *color* 是下面定义的 **color** 类型的对象。如果未指定，*nPixels* 默认为 1（即单个像素）。如果为 *nPixels* 指定的值与设备中的实际 RGB 像素总数不匹配，这不是问题；如果 *nPixels* 小于设备像素总数，则仅第一个*nPixels* 将被设置为 *color*；如果 *nPixels* 大于设备像素总数，设备将忽略额外的输入
  
* `void set(Color *color, int nPixels)`

  * 将多像素设备中每个像素的颜色单独设置为 **color** 数组 *\*color*，大小为 *nPixels* 的 **color** 数组 *\*color* 中指定的颜色值，其中设备的第一个像素设置为中的值 *color\[0\]*，第二个像素设置为中的值 *color\[1\]*...最后一个像素设置为中的值 *color\[nPixels-1\]*。与上面类似，如果指定的值没有问题 *nPixels* 与设备中实际 RGB 像素总数不匹配

在上述两种方法中，颜色都存储在 32 位 **color** 对象中，该对象配置为保存三个 8 位 RGB 值以及一个可用于限制 LED 电流的 5 位值。**color** 对象可以实例化为单个变量（例如 `Dot::Color myColor;` ）或数组（例如 `Dot::Color myColors[8];` ）。请注意，**Dot** 类使用的 **color** 对象的范围仅限于 **Dot** 类本身，因此你需要使用 full-合格的类名称 "Dot::Color"。创建 **color** 对象后，可以使用以下两种方法之一设置它存储的颜色：
  
  * `Color RGB(uint8_t r, uint8_t g, uint8_t b, uint8_t driveLevel=31)`

    * 其中 *r*、*g* 和 *b* 表示 0-255 范围内的 8 位红色、绿色和蓝色值，*driveLevel* 表示 5 位值 [0-31]，用于限制 LED 电流从 0（无电流）到 31（最大电流，这是默认值）。通过将 *driveLevel* 设置为小于 31 的值来限制 LED 电流，可以提供一种控制每个像素全彩 LED 亮度的无闪烁方式。
    * 示例： `myColor.RGB(128,128,0)` 使用红色和绿色 LED 的 50% 占空比将 myColor 设置为半亮度黄色（即 128/256）
    * 示例： `myColor.RGB(255,255,0,16)` 通过将像素的 LED 电流限制为其最大值的 50%（即 16/32），将 myColor 设置为半亮度黄色
      
  * `Color HSV(float h, float s, float v, double drivePercent=100)`
    
    * 其中*h* 为色调，范围为 0-360；*s* 为饱和度百分比从 0-100；*v* 为 0-100 之间的亮度百分比。这些值将转换为等效的 8 位 RGB 值 (0-255)，以便存储在 *color* 对象中。*drivePercen* 参数以与上面 *driveLevel* 相同的方式控制电流，不同之处在于它不是指定为 0-31 的绝对值，而是指定为百分比从 0 到 100（默认）
    * 示例： `myColor.HSV(120,100,50)` 使用 50% 占空比将 myColor 设置为半亮度、完全饱和的绿色
    * 示例： `myColor.HSV(120,100,100,50)` 通过将像素的 LED 电流限制为其最大值的 50%，将 myColor 设置为半亮度的完全饱和绿色
      
请注意，上述两种方法都会返回完整的 **color** 对象本身，因此可以在需要 **color** 对象的任何地方使用：例如： `Dot p(5,6); Dot::Color myColor; p.set(myColor.RGB(255,215,0))` 将连接到引脚 5 和 6 的单个像素设备的颜色设置为亮金色。

 **Dot** 类还支持以下类级方法作为创建颜色的便捷替代方法：
  
* `static Color RGB(uint8_t r, uint8_t g, uint8_t b, uint8_t driveLevel=31)`
  * 相当于 `return(Color().RGB(r,g,b,driveLevel));`
  * 示例： `Dot p(8,11);  p.set(Dot::RGB(0,0,255),8);` 将 8 像素设备中每个像素的颜色设置为蓝色

* `static Color HSV(float h, float s, float v, double drivePercent=100)`
  * 相当于 `return(Color().HSV(h,s,v,drivePercent));`
  * 示例： `Dot::Color c[]={Dot::HSV(120,100,100),Dot::HSV(60,100,100),Dot::HSV(0,100,100)};` 创建红黄绿交通灯图案

与 **Pixel** 类不同，**Dot** 类*不是*使用 ESP32 的 RMT 外设，因此没有限制你可以实例化 **Dot** 类对象的数量。此外，由于时钟信号是由 **Dot** 类本身生成的，因此无需设置定时参数，也不需要 *setTiming()* 方法。

## *WS2801_LED(uint8_t dataPin, uint8_t clockPin [, spi_host_device_t spiBus])*

创建此**类**的实例会将指定的引脚配置为输出适用于控制基于 *WS2801* 协议的双线可寻址 RGB LED 设备的波形信号，该设备可具有任意数量的像素。参数及其默认值（如果未指定）如下：

  * *dataPin* - RGB 数据信号的输出引脚；通常连接到可寻址 LED 设备的“数据”输入端

  * *clockPin* - RGB 时钟信号的输出引脚；通常连接到可寻址 LED 设备的“时钟”输入端

  * *spiBus* - 指定用于信号生成的 SPI 总线控制器。允许的值和默认值取决于设备：

    * 对于 ESP32，*spiBus* 可以设置为 **SPI2_HOST** 或 **SPI3_HOST**。如果未指定，*spiBus* 默认为 **SPI2_HOST**。

    * 对于 ESP32-S2/S3，*spiBus* 可以设置为 **SPI2_HOST** 或 **SPI3_HOST**。如果未指定，*spiBus* 默认为 **SPI3_HOST**

    * 对于 ESP32-C3/C5/C6，*spiBus* 只能设置为 **SPI2_HOST**，因此如果未指定，则默认为 **SPI2_HOST**

设置像素颜色的两种主要方法是：

* `void set(Color color, int nPixels=1)`

  * 将单像素设备中单个像素的颜色，或等效地，将多像素设备中前 *nPixels* 个像素的颜色设置为 *color*，其中 *color* 是下面定义的 **Color** 类型的对象。如果未指定，*nPixels* 默认为 1（即单个像素）。*nPixels* 的值与设备中实际 RGB 像素的总数不匹配不会有问题；如果 *nPixels* 小于设备像素的总数，则只会将前 *nPixels* 个像素的颜色设置为 *color*；如果 *nPixels* 大于设备像素总数，设备将忽略额外的输入。

* `void set(Color *color, int nPixels)`

  * 将多像素设备中每个像素的颜色分别设置为大小为 *nPixels* 的 **Color** 数组 *color* 中指定的颜色值。其中，设备的第一个像素设置为 *color[0]* 中的值，第二个像素设置为 *color[1]* 中的值，……最后一个像素设置为 *color[nPixels-1]* 中的值。与上述类似，如果 *nPixels* 指定的值与设备中实际的 RGB 像素总数不匹配，也不会出现问题。

在上述两种方法中，颜色都存储在一个 24 位 **Color** 对象中，该对象配置为存储三个 8 位 RGB 值。 **Color** 对象可以实例化为单个变量（例如 `WS2801_LED::Color myColor;`），但不应直接实例化为简单的数组变量（例如 `WS2801_LED::Color myColors[8];`）。这是因为 **WS2801_LED** 需要 SPI 总线的 DMA 功能来确保颜色数据正确传输到 LED，因此 **Color** 数组必须创建在 32 位对齐且支持 DMA 的内存中。

ESP32 操作系统提供了两种创建存储在 32 位对齐且支持 DMA 的内存中的变量的方法。第一种方法是在实例化简单数组时添加属性 `DMA_ATTR`，如下所示：`DMA_ATTR WS2801_LED::Color c[5];`。

但是，此属性只能在任何函数之外创建全局变量时使用。 `DMA_ATTR` 不能用于局部变量，因为局部变量是在运行时添加到堆栈中的，操作系统无法保证 32 位对齐。要使用 32 位对齐的 DMA 内存创建 **Color** 对象本地数组，请实例化一个指向 **Color** 数组的*指针*，并将其设置为以下静态类函数返回的内存段：

* `static Color *getMem(size_t nColors)`

  * 返回一个指向动态分配的 32 位对齐的 DMA 内存段的 **Color** 指针，该内存段的大小足以容纳 *nColors* 个对象

  * 例如：`WS2801_LED::Color *myColors = WS2801_LED::getMem(8)` 将 *myColors* 设置为包含 8 个 **Color** 对象的数组

  * 请记住，与任何动态分配的内存一样，当不再需要时，必须释放它（例如，`free(myColors);`）。此类内存不会在函数结束时自动销毁！

请注意，**WS2801_LED** 类使用的 **Color** 对象的作用域仅限于 **WS2801_LED** 类本身，因此在创建 Color 变量时，需要使用完全限定类名“WS2801_LED::Color”。

创建 **Color** 对象后，可以使用以下两种方法之一设置其存储的颜色：

  * `Color RGB(uint8_t r, uint8_t g, uint8_t b)`

    * 其中 *r*、*g* 和 *b* 分别表示 0-255 范围内的 8 位红色、绿色和蓝色值。

    * 例如：`myColor.RGB(128,128,0)` 将 myColor 设置为黄色，亮度为一半，红绿 LED 的占空比为 50%（即 128/256）。
  *  `Color HSV(float h, float s, float v)`

    * 其中，*h* 表示色调，取值范围为 0-360；*s* 表示饱和度，取值范围为 0-100%；*v* 表示亮度，取值范围为 0-100%。这些值会被转换为等效的 8 位 RGB 值 (0-255)，并存储在 *Color* 对象中。

    * 例如：`myColor.HSV(120,100,50)` 将 myColor 设置为半亮度、完全饱和的绿色，占空比为 50%。

请注意，以上两种方法都会返回完整的 **Color** 对象本身，因此可以在任何需要 **Color** 对象的地方使用：例如：`WS2801_LED p(5,6); WS2801_LED::Color myColor;` `p.set(myColor.RGB(255,215,0))` 将连接到引脚 5 和 6 的单个像素设备的颜色设置为亮金色。

**WS2801_LED** 类还支持以下类级方法，作为创建颜色的便捷替代方案：

* `static Color RGB(uint8_t r, uint8_t g, uint8_t b)`

  * 等效于 `return(Color().RGB(r,g,b));`

  * 例如：`WS2801_LED p(8,11);` `p.set(WS2801_LED::RGB(0,0,255),8);` 将 8 像素设备中的每个像素的颜色设置为蓝色。

* `static Color HSV(float h, float s, float v)`

  * 等效于 `return(Color().HSV(h,s,v));`

  * 例如：`WS2801_LED p(8,11); p.set(WS2801_LED::HSV(240,100,75),8);` 将 8 像素设备中的每个像素的颜色设置为 75% 亮度下的深蓝色。

最后需要注意的是，*WS2801* 设备的刷新率比 *DotStar* 设备慢得多。默认情况下，**WS2801_LED** 类会将每个实例化对象的 SPI 时钟频率设置为 2MHz，这与 WS2801 芯片的规格相符。但是，如果需要，您可以使用以下基于成员的方法为任何给定的 **WS2801_LED** 对象设置不同的 SPI 时钟频率：

* `void setTiming(uint32_t freq)`

  * 将像素传输频率设置为 *freq*（单位为 Hz）

  * 例如：`WS2801_LED p(8,11); p.setTiming(1000000); p.set(WS2801_LED::RGB(0,0,255),8);` 将传输频率更改为 1MHz，然后再将 8 像素设备中的每个像素的颜色设置为蓝色

#### WS2801 设备和 SPI 总线 - 技术细节（可选阅读）

*DotStar* 和 *WS2801* 设备使用几乎相同的双线协议来表示和传输像素数据。然而，尽管两款器件都采用相同的 3 字节格式来控制每个像素的 24 位 RGB 颜色，但 *DotStar* 器件还包含第四个字节，用于控制每个像素内置的 32 级驱动电流限制器（*WS2801* 器件不具备此功能）。重要的是，由于 32 级电流限制器仅需 5 位即可表示，因此 *DotStar* 器件的第四个字节中还剩下 3 位，用于编码给定的 4 字节传输是否应被器件解释为有效的像素颜色，或者是否应被解释为传输开始指示符。传输开始指示符用于将器件对已传输数据的锁存重置回该像素链中的第一个像素。

由于 *WS2801* 每个像素仅使用 3 个字节，因此没有额外的位来编码传输开始指示符。相反，当时钟信号被拉低超过 500 微秒时，WS2801 器件会将已发送数据的锁存状态重置回灯串中的第一个像素。在单处理器微控制器常用的简单单线程操作系统中，可以直接在软件中正确实现时钟信号。然而，ESP32 使用的是能够支持多处理器微控制器的完整多线程操作系统。因此，WS2801 协议无法完全通过软件实现，因为操作系统在像素数据传输期间执行的任何 CPU 任务切换都可能（而且经常会）使时钟信号保持低电平超过 500 微秒，从而导致像素器件在传输过程中重置。

为了解决这个问题，**WS2801_LED** 类并没有在软件中实现 WS2801 协议，而是使用 ESP32 内置的 SPI 外设来传输数据。如果 SPI 外设配置为使用 DMA 存储器，则数据传输在*硬件*级别进行，因此不会受到操作系统在传输过程中执行的任何 CPU 任务切换的影响。

ESP32、ESP32-S2 和 ESP-S3 设备具有 4 个 SPI 总线控制器（SPI0 至 SPI3）。SPI0 和 SPI1 分别保留用于与闪存和 SPI RAM 通信。这使得 SPI2 和 SPI3 可用作通用 SPI 总线，供任何程序使用。在 ESP32 上，Arduino-ESP32 内核使用 SPI3 总线实例化其全局 SPI 对象，因此 **WS2801_LED** 类默认使用 SPI2 总线。相比之下，在 ESP32-S2 和 ESP32-S3 上，Arduino-ESP32 内核使用 SPI2 总线实例化其全局 SPI 对象，因此 **WS2801_LED** 类默认使用 SPI3 总线。
ESP32-C3、ESP32-C5 和 ESP32-C6 设备只有 3 个 SPI 总线控制器（SPI0 至 SPI2）。由于 SPI0 和 SPI1 分别用于与闪存和 SPI RAM 通信，因此只有 SPI2 可供 **WS2801_LED** 类以及 Arduino-ESP32 内核的全局 SPI 对象实例使用。

**WS2801_LED** 类的设计旨在最大程度地减少与 Arduino-ESP32 内核的全局 SPI 对象之间的任何干扰，因此您的程序可以同时使用同一 SPI 总线（SPI2 或 SPI3，如果可用）连接任何外部 SPI 设备（例如基于 SPI 的以太网模块）和 **WS2801_LED** 类。

如上所述，为了确保数据传输完全在硬件层面完成，无需占用 CPU 资源，待传输的数据需要存储在支持 DMA 的内存中。静态函数 `getMem()` 的作用就在于此——它返回一个指向动态分配的 32 位对齐的 DMA 内存段的指针，该内存段适用于 SPI DMA 硬件。

### 示例草图

一个完整的示例演示了如何在 HomeSpan 程序中使用 **Pixel**、**Dot** 和 **WS2801_LED** 类来控制 RGB 像素设备、RGBW 像素设备、RGB DotStar 设备和 RGB WS2801 设备，所有这些设备都可以通过 iPhone 上的“家庭”App 进行控制。该示例位于 Arduino IDE 的 [*文件→示例→HomeSpan→其他示例→像素*](../examples/Other%20Examples/Pixel/Pixel.ino)。第二个示例演示了如何实现带有单独的 Home App 控制 RGB 和 WC LED 的 RGBWC 像素灯带，可以在 Arduino IDE 的 [*文件→示例→HomeSpan→其他示例→Pixel-RGBWC*](../examples/Other%20Examples/Pixel-RGBWC/Pixel-RGBWC.ino).

如需更完整地展示 Pixel 库，请查看 [HomeSpan 项目](https://github.com/topics/homespan)上的 [节日灯](https://github.com/HomeSpan/HolidayLights)。此草图演示了如何使用 Pixel 库通过 60 像素 RGBW 条生成各种特殊效果。该草图还展示了使用 HomeSpan 的 [自定义特征宏](Reference.md#custom) 来实现在 Eve 应用中使用的特效“选择器”按钮。

---

[↩️](../README.md#resources) 返回欢迎页面
