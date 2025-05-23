## ❗最新更新 - HomeSpan 2.1.2 (2025年5月8日)

### 更新和修正

* **新增自定义服务的 UUID 验证**
  * 如果发现无效的服务 UUID，则在启动时在 CLI 中报告错误
  * 类似于现有的自定义特性的 UUID 验证

* **将示例 sketch *RemoteDevice8286.ino* 重命名为 *RemoteDevice8266.ino***
  * 更正了文件名中一个长期存在的拼写错误

* **修改了 OTA 更新，使 HomeSpan 仅在上传新的 *sketch*** 时才检查其 Magic Cookie
  * 避免使用 OTA 上传 SPIFFS 数据时 OTA 中止

* **重构了处理来自 HomeKit 的 PUT 特性请求的 JSON 解析逻辑**
  * 现在正确支持 JSON 字符串值中使用的任何 JSON 允许的 Unicode 字符，从 U+0020 到U+10FFFF

    * 允许基于字符串的特性包含转义引号、转义斜线和反斜线，以及任何 JSON 令牌字符 *,:[]{}*，这些字符以前会导致解析错误。
  * 现在还允许基于空字符串的特性（以前会导致解析错误）。

* **在 Characteristics 中添加了新的 `setMaxStringLength(uint8_t n)` 方法**
  * 允许用户将基于字符串的特性的最大长度从 HAP 默认的 64 更改为 *n*（小于 256）。
  * 虽然 HAP 指定了该值，但 HomeKit 似乎并未使用该值，因此该方法似乎没有必要。

* **添加了新的 *homeSpan* 方法 `assumeTimeAcquired()`**
  * 调用此方法会告知 HomeSpan 假定您已使用自己的代码获取时间。
  * 如果您不想在启用网络日志时指定 *timeServerURL*，而是希望手动获取时间，则此方法非常有用。

* **添加了新的 *homeSpan* 方法 `setGetCharacteristicsCallback(void (*func)(const char *getCharList))`**
  * 设置一个可选的用户自定义回调函数 *func*，每当 HomeSpan 收到来自 HomeKit 的 *GET /characteristics* 请求时，该函数都会被调用。
  * 每次在 iPhone 或 Mac 上打开“家庭”应用时，HomeKit 通常会将此请求发送给所有配对的设备。
  * 此回调在 HomeSpan 必须使用单独的“宝贵”进程读取传感器类特性的当前状态的情况下非常有用，该进程应仅在需要时调用，而不是在服务的 `loop()` 方法中持续更新。
  * 函数 *func* 必须是 void 类型，并接受一个 *const char \** 类型的参数，HomeSpan 将 HomeKit 在其 HTTP *GET* 请求中提供的特性 AID/IID 对列表传递给该参数。
  * *getCharList* 可用于判断 HTTP *GET* 请求是否包含 AID/IID 对。对于任何特定的特性
    * 这允许用户根据 HomeKit 请求的特定特性对回调进行操作
    * 请参阅**新的 SpanCharacteristic 辅助方法 `foundIn(const char *getCharList)`**，该方法根据特定特性的 AID/IID 是否在 *getCharList* 中找到而返回 *true* 或 *false*。
    * 为了完整性，**还向 SpanAccessory、SpanService 和 SpanCharacteristic 类**分别添加了 `uint32_t getAID()` 方法。

* **在 *HomeSpan.cpp* 中明确添加了 `#include <mutex>`，以解决与 Arduino-ESP32 v3.2.0 的兼容性问题。**

* **修复了 `Pixel::getPin()` 中报告通道号而不是引脚号的错误。**

有关此版本中包含的所有更改和错误修复的详细信息，请参阅 [发布](https://github.com/HomeSpan/HomeSpan/releases) 更新。


## ❗最新更新 - HomeSpan 2.1.1 (2025 年 2 月 10 日)

### 集成对 OTA 分区回滚的支持

* **用户现在可以配置 HomeSpan，如果新草图在验证之前使设备崩溃，则自动将通过 OTA 上传的新草图回滚到以前的版本**

  * 添加新的头文件 `SpanRollback.h`
    * 当包含在用户草图的顶部时，这将禁用 ESP32-Arduino 库在启动时执行的任何新更新的 OTA 分区的自动验证
    * 用户可以在软件中*手动*验证他们的草图，如果新草图未标记为有效，则允许设备自动回滚到之前的草图

  * 添加新的 homeSpan 方法 `markSketchOK()`，允许用户在通过 OTA 上传新草图后将当前正在运行的分区标记为有效
  * 添加新的 homeSpan 方法 `setPollingCallback()`，允许用户添加 HomeSpan 的回调函数在第一次调用 `poll()` 完成后调用 *一次*
  * 添加新的 CLI 'p' 命令，将分区完整表打印到串行监视器
  * 添加新的 homeSpan 方法 `setCompileTime()`，允许用户将 **草图** 的编译日期/时间设置为任意字符串
  * 有关如何使用 OTA 回滚的详细信息，请参阅 [HomeSpan OTA](docs/OTA.md) 页面

### HomeSpan 看门狗定时器

* **用户现在可以配置 HomeSpan 以添加看门狗任务，如果设备冻结或进入无限循环阻止正常 HomeSpan 操作，则该任务将重新启动设备**

  * 添加新的 homeSpan 方法 `enableWatchdog(uint16_t nSeconds)`
    * 创建单独的 HomeSpan **任务看门狗定时器**，旨在触发设备重新启动，如果没有至少每 *nSeconds* 定期重置一次

  * 添加新的 homeSpan 方法 `resetWatchdog()`，用于定期重置HomeSpan 看门狗定时器
  * 此方法已根据需要嵌入到所有 HomeSpan 库函数中
  * 添加新的 homeSpan 方法 `disableWatchdog()`，用于在启用 HomeSpan 看门狗定时器后将其禁用
  * 请参阅 [HomeSpan 看门狗定时器](docs/WDT.md) 页面，了解有关 HomeSpan 和其他系统看门狗定时器的完整讨论

### 其他更新

* 添加 homeSpan 方法 `Span& useEthernet()`，强制 HomeSpan 使用以太网而不是 WiFi，即使在调用 `homeSpan.begin()` 之前尚未调用 ETH 或未找到以太网卡

### 错误修复！

* **修复了 HomeSpan 2.1.0 中引入的一个错误，该错误会根据代码的编译方式错误地初始化 WiFi 和以太网堆栈**

有关此更新中包含的所有更改和错误修复的详细信息，请参阅 [发布](https://github.com/HomeSpan/HomeSpan/releases)。



## ❗最新更新 - HomeSpan 2.1.0 (2024/12/27)

* **集成对以太网连接的支持！**

  * 不需要新的 homeSpan 方法。相反，在启动期间，HomeSpan 会检查您是否已指示 ESP32 建立以太网连接，如果是，它将切换到“以太网模式”，而不是尝试通过 WiFi 连接到您的网络
  * 一旦进入以太网模式，HomeSpan 就会自定义串行监视器和 Web 日志的一些输出，以便清楚地看到正在使用以太网而非 WiFi 连接
  * HomeSpan 处理所有连接/断开/重新连接的报告，就像它通常处理 WiFi 连接一样
  * 要建立以太网连接，只需使用 Arduino-ESP32 的 ETH 库，在草图中调用 `ETH.begin()`，并使用适合您的以太网板的参数（假设 Arduino-ESP32 库支持您的主板）
    * 你必须在调用 `homeSpan.begin()` 之前调用 `ETH.begin()`
    * 你**不需要**在草图中包含 `ETH.h`
    * 请注意，Arduino-ESP32 ETH 库支持直接连接 PHY 以及独立的基于 SPI 的以太网板
  * 添加了新的 homeSpan 方法 `setConnectionCallback()`，它是 `setWifiCallbackAll()` 方法的重命名版本（现已弃用，见下文），以反映此方法可以用于以太网和 WiFi 连接的事实

* **WiFi 增强功能和新的 WiFi 管理方法**

  * 当连接到具有多个接入点的 WiFi 网状网络时，HomeSpan 现在**自动连接到具有最强 WiFi 信号**（即最大 RSSI）的接入点
    * 以前，HomeSpan 只会连接到它找到的第一个与用户指定的 SSID 匹配的接入点，即使具有相同 SSID 的其他接入点信号更强
    * HomeSpan 当前连接的接入点的 BSSID（6 字节 MAC 地址）在 Web 日志以及串行监视器中提供，以响应“s”CLI 命令

  * 添加了新的 homeSpan 方法 `setConnectionTimes()`，允许用户微调 HomeSpan 在尝试连接 WiFi 网络时等待每次连接尝试的时间
  
  * 添加了新的 homeSpan 方法 `setWifiBegin()`，允许用户在尝试连接到 WiFi 网络时创建 HomeSpan 调用的替代函数，**而不是** `WiFi.begin()`

    * 提供创建自定义项的能力，例如连接到企业网络，或在建立连接时更改 WiFi 功率（某些 ESP32 开发板的 WiFi 无线电配置错误时需要这样做）
    
  * 添加了新的 homeSpan 方法 `enableWiFiRescan()`，使 HomeSpan 定期重新扫描与配置的 SSID 匹配的所有接入点，并自动切换到信号最强的接入点
     * 在重新启动网状网络并且 HomeSpan 最初重新连接到更远的接入点后很有用，因为更近且信号更强的接入点尚未完全重新启动
       
  * 添加了新的 homeSpan 方法 `addBssidName()`，允许用户根据其 6 字节 BSSID 地址为 WiFi 网状网络中的每个接入点创建可选的显示名称
     * 定义后，HomeSpan 将同时显示此名称和 BSSID每次在串行监视器上显示信息或写入 Web 日志时，都会有一个接入点
     
       
  * 请参阅 [API 参考](docs/Reference.md) 页面了解完整详情，以及新的 [HomeSpan WiFi 和以太网连接](docs/Networks.md) 页面了解 HomeSpan 连接选项的高级讨论

* **弃用**
  * `setWifiCallbackAll()` 已被弃用并重命名为 `setConnectionCallback()`，以反映此回调可用于 WiFi 和以太网连接的事实
  * `setWifiCallback()` 已被弃用 --- 应改用更通用的 `setConnectionCallback()`
    * 要求升级任何现有回调以添加一个表示连接尝试次数的整数参数，类似于 `setWifiCallbackAll()` 和现在的 `setConnectionCallback()` 的工作方式
  * `setWifiCallbackAll()` 和 `setWifiCallback()` 都将在 HomeSpan 的未来版本中删除。请更新您的草图以避免与这些未来版本不兼容

* **新的 CLI 命令**
  
  * 'D' - 强制 HomeSpan 断开连接，然后自动重新连接到配置的 WiFi 网络
  * 'Z' - 扫描用户的 WiFi 网络环境并在串行监视器上显示有关每个 SSID 的信息（包括具有多个接入点广播相同 SSID 的网状网络的每个 BSSID）
  * 请参阅 [命令行界面 (CLI)](docs/CLI.md) 页面了解完整详细信息

* **新的多线程管理**

  * 使 Web 日志写入/读取线程安全
    * 修复了与显示 Web 日志和写入日志记录之间的竞争条件相关的潜在错误，当 HomeSpan 在启动时创建单独的线程来处理与 NTP 服务器的初始联系时，记录找到的时间

  * 使 HomeSpan 自动轮询线程安全
    * 添加了两个新宏 `homeSpanPAUSE` 和 `homeSpanRESUME`，允许用户在 HomeSpan 轮询任务完成当前运行后暂时暂停该任务
    * 允许用户修改 HomeSpan 特性并从单独的线程执行任何其他 HomeSpan 功能，而不必担心同时运行 HomeSpan 轮询时出现不一致的情况
    * 通常用于当您的草图调用 `homeSpan.autoPoll()` 在单独的后台任务中运行 HomeSpan 轮询*并且*您还想通过在主 Arduino `loop()` 中使用 `getVal()` 和 `setVal()` 对现有的 HomeSpan 特性进行单独的修改时（而不是或除了在其服务循环中修改这些特性）
      
  * 请参阅 [API 参考](docs/Reference.md) 页面了解完整详情，以及新的 [多线程](examples/Other%20Examples/MultiThreading) 示例，该示例演示了如何使用这些宏来实现多线程

* **Web 日志增强功能**

  * Web 日志现在可以从浏览器自动刷新
    * 如果来自浏览器的 Web 日志请求包含查询字符串 *refresh=N*，HomeSpan 将向其返回给浏览器的 HTML 添加一个 HTTP 刷新响应标头，使其每 *N* 秒自动刷新一次 Web 日志请求
    * 例如： *http<nolink>://homespan-4e8eb8504e59.local/status?refresh=10* 

  * Web 日志请求现在不区分大小写
    * 例如： *http<nolink>://homespan-4e8eb8504e59.local/STATUS?REFRESH=10* 相当于上面
 
* **问题修复**
  * 修复了在草图中使用 homeSpan 方法 `controllerListBegin()` 和 `controllerListEnd()` 时阻止编译的潜在错误
 
* **已知问题**
  * 在 Arduino-ESP32 Board Manager v3.1.0 下，无法调用外部 NTP 服务器来设置 C6 芯片上的时间（所有其他芯片不受影响）。详情请参阅 https://github.com/espressif/arduino-esp32/issues/10754。
      
有关此更新中包含的所有更改和错误修复的详细信息，请参阅 [发布](https://github.com/HomeSpan/HomeSpan/releases)。


## ❗最新更新 - HomeSpan 2.0.0-rc.1 (9/27/2024)

* **与 [Arduino-ESP32 开发板管理](https://github.com/espressif/arduino-esp32) 版本 3 兼容**

  * Arduino-ESP32 主板管理器第 3 版基于乐鑫的 IDF-5，与基于乐鑫的 IDF-4 的 Arduino-ESP32 主板管理器第 2 版不向后兼容
  * HomeSpan 2.0.0-rc.1 的主要重点是（最低限度地）重构 HomeSpan 1.9.1，以使其能够在 Arduino-ESP32 板管理器第 3 版下运行 ---
    * 同时**保留所有在 HomeSpan 1.9.1 下编写的 HomeSpan 草图的向后兼容性**
    * 尽管乐鑫在 IDF-4 和 IDF-5 之间做出了许多重大变更，
    * Arduino-ESP32 开发板管理 2 版和 3 版之间引入了许多其他重大变更
    * **需要 Arduino-ESP32 开发板管理 3.0.2 或更高版本**
      
* **支持 ESP32-C6**
  
  * Arduino-ESP32 Board Manager 版本 3 增加了对乐鑫的 ESP32-C6 和 ESP32-H2 芯片的支持
    * HomeSpan 支持使用 C6 芯片，因为它包含 WiFi 无线电，这是 HomeSpan 目前的要求
    * HomeSpan 不支持使用 H2 芯片，因为它没有 WiFi 无线电（仅包含 Thread 无线电）
 
* **HomeSpan Pixel 和 RFControl 库的扩展功能**

  * 由于乐鑫弃用了 IDF4 版本的 RMT 库，并在 IDF-5 中将其替换为一个全新的库，HomeSpan **Pixel** 和 **RFControl** 库已完全编写和升级：
  * 您现在可以在同一个草图中实例化 **Pixel** 和 **RFControl** 对象（以前这些类彼此不兼容，不能在同一个草图中使用）
  * **Pixel** 类还添加了各种新功能，支持更多类型的 Pixel 设备，包括
    * 升级版 [PixelTester](examples/Other%20Examples/PixelTester) 草图，引导用户选择并测试适合其设备的正确 *pixelType*
    * 新的 [Pixel-RGBWC](examples/Other%20Examples/Pixel-RGBWC) 示例，演示如何实现 RGBWC Pixel 灯带，并为 RGB 和 WC LED 设置单独的 Home App 控件
    * 请参阅 [可寻址 RGB LED](docs/Pixels.md) 页面了解完整详情

* ❗**存储警告** ❗
  * Arduino-ESP32 Board Manager 版本 3 的占用空间**比版本 2 大得多**
  * **在 1.9.1 下编译的相同 HomeSpan 草图在 HomeSpan 2.0.0-rc.1 下将大约大 200K**
  * HomeSpan 草图将不再适合 *默认* 分区方案，该方案仅为每个 OTA 分区分配约 1.3MB
  * 草图必须在更大的分区方案下编译，例如 *Minimal SPIFFS*，它提供 1.9MB OTA 分区
  * **这对通过 OTA 升级草图有影响（有关详细信息，请参阅发行说明）**

有关此更新中包含的所有更改和错误修复的详细信息，请参阅 [发布](https://github.com/HomeSpan/HomeSpan/releases)。

## ❗最新更新 - HomeSpan 1.9.1 (07/03/2024)

* **HomeSpan 现在支持 *标签长度值 ("TLV8")* 特性！**

  * 添加了新的、完全集成的 `TLV8()` 类库，用于创建和管理 TLV8 对象
  * 包括处理标准字节流值以及字符串、数值、零长度标签和子 TLV 的方法
  * 利用标准 C++ 迭代器轻松访问读取和写入 TLV8 记录
  * 添加了新的 `Characteristic` 方法 `getTLV()`、`getNewTLV()` 和 `setTLV()`
  * 添加了新的 `CUSTOM_CHAR_TLV8()`，允许轻松创建自定义 TLV8 特性
  * 包括新的教程  [示例 22 - TLV8 特性](examples/22-TLV8_Characteristics/22-TLV8_Characteristics.ino)，演示了 `TLV8()` 类和 TLV8 特性的使用
  * 请参阅新的 [TLV8 特性](docs/TLV8.md) 页面了解完整内容详细信息和文档
     
* **新的 *DisplayOrder* TLV8 特性**

  * 利用 HomeSpan 的新 `TLV8()` 库
  * 允许您指定电视服务输入源在“家庭”应用中显示的确切顺序
  * 请参阅教程 [示例 22 - TLV8 特性](examples/22-TLV8_Characteristics/22-TLV8_Characteristics.ino) 了解详情
 
* **新 *AccessoryIdentifier* 教程**

  * 演示如何触发配件的标识符特征，可选择用于在初始配对期间帮助识别设备
  * 参见教程 [示例 21 - 配件标识符](examples/21-AccessoryIdentifier/21-AccessoryIdentifier.ino)

* **增加了对自定义 Pixel 芯片的支持**

  * 新的构造函数 `Pixel(uint8_t pin, [pixelType_t pixelType])` 允许您设置将颜色传输到像素芯片的顺序，其中 *pixelType* 是以下之一：
    * PixelType::RGB, PixelType::RBG, PixelType::BRG, PixelType::BGR, PixelType::GBR, PixelType::GRB
    * PixelType::RGBW, PixelType::RBGW, PixelType::BRGW, PixelType::BGRW, PixelType::GBRW, PixelType::GRBW*
  * 弃用以前的构造函数 `Pixel(uint8_t pin, boolean isRGBW)`
    * 此构造函数将继续工作，但您将在编译期间收到已弃用的警告
    * 用户应切换到新的构造函数，以避免与 HomeSpan 未来版本的潜在兼容性问题
  * 添加了新方法 `boolean isRGBW()`
    * 如果 Pixel 被构建为 RGBW，则返回 *true*；如果仅被构建为 RGB（即没有白色 LED），则返回 *false*
  * 添加了新的 [灯带测试](examples/Other%20Examples/PixelTester/PixelTester.ino) 草图（位于 *Other Examples* 下），以帮助确定任何 LED 灯带的 *pixelType*
  * 有关详细信息，请参阅 [灯带](docs/Pixels.md) 页面
    
* **新增读取和设置服务和特征的 IID 的功能**

  * 添加新的 `SpanService` 方法 `getIID()`，用于返回服务的 IID
  * 添加新的 `SpanCharacteristic` 方法 `getIID()`，用于返回特征的 IID
  * 添加新的 `homeSpan` 方法 `resetIID(int newIID)`，用于重置当前附件的 IID 计数
  * 请参阅 [API 参考](docs/Reference.md) 了解详情
  
* **新增读取控制器配对数据的功能（仅适用于高级用例）**

  * 添加新的 `homeSpan` 方法 `controllerListBegin()` 和 `controllerListEnd()`，将迭代器返回到 HomeSpan 控制器数据记录的内部链接列表
  * 添加新方法来读取每个控制器的配对数据：
    * `getID()` - 返回指向控制器 36 字节设备 ID 的指针
    * `getLTPK()` - 指向控制器 32 字节长期公钥的指针
    * `isAdmin()` - 如果控制器具有管理员权限，则返回 true，否则返回 false
  * 添加新的 `homeSpan` 方法 `setControllerCallback()` 来设置可选回调函数，每当添加、删除或更新控制器时，HomeSpan 都会调用该函数
  * 请参阅 [API 参考](docs/Reference.md) 了解详情
 
* **HomeSpan 现在支持 *write-response ("WR")* 协议**
  * 添加了对 HomeKits 的 *write-response ("WR")* 协议的自动处理*
    * 对于 HomeSpan 目前支持的任何特征都不需要，但对于实验和使用自定义特征很有用
  * 使用 `setVal()` 时添加了额外的检查
    * 如果在“家庭”应用发送该值的更新请求的同时，在 `update()` 方法中调用 `setVal()` 来更改特征的值，则串口监视器上会输出警告消息
    * 如果在 `update()` 中调用 `setVal()` 来更改特征的值以响应来自“家庭”应用的 *write-response* 请求，则不适用
   
* **将 `getLinks()` SpanService 方法转换为模板函数**
  * 允许用户自动将返回向量的元素转换为任何特定的服务类型
  * 还添加了一个可选参数来限制返回向量的元素以匹配指定的 HomeSpan 服务
  * 请参阅 [API 参考](docs/Reference.md) 了解详情
 
* **新功能可暂停 ServoPin 的脉冲生成**
  * 调用 ServoPin 的 `set(NAN)` 可暂停脉冲生成，这（对于大多数模拟伺服）可让电机自由旋转
  * 调用 `set(position)`，其中 *position* 等于所需度数，可重新启动脉冲生成并相应地设置伺服位置
   
* **重构客户端/插槽管理以节省内存并为将来集成以太网支持做好准备**
  * 客户端/套接字连接的固定数组被动态链接列表取代
  * 串口接口现在仅显示活动客户端连接（而不是客户端插槽的固定列表）
  * **已弃用** `homeSpan.reserveSocketConnections()`，因为不再需要它
        
* **修复了 1.9.0 中引入的 bug，该 bug 阻止 `homeSpan.setPairingCode()` 保存（并随后使用）请求设置配对代码**
  * 此方法现在以静默方式运行，除非提供了无效的配对代码，在这种情况下会向串口监视器报告错误并且 *草图会停止*
  * 使用 CLI 'S' 命令或通过接入点设置配对代码的过程保持不变——确认消息仍会输出到串口监视器，并且错误不会导致草图停止

* **修复了 1.9.0 中引入的内存泄漏问题，该问题会导致无法释放验证新连接时创建的一小块临时内存块**
  * 使用家庭中心时没有实际影响，因为 HomeKit 仅创建少量永久连接
  * 不使用家庭中心时会产生重大影响，因为“家庭”应用会反复断开并重新建立连接，导致堆内存缓慢消耗，然后在几天后出现设备内存不足故障（请注意，不正式支持在没有 家庭中心的情况下使用 HomeSpan）

* **修复了 SpanPoint 中的潜在错误**
  * 如果 Span 热点的任何实例具有 *receiveSize=0*，则在将 **Span 热点** 配置信息打印到串口监视器（"i" CLI 命令）时，HomeSpan 会崩溃
  * 此错误之前从未出现过，因为所有 **Span 热点** 都是基于接收数据的，因此具有非零的 *receiveSize*
 
* **删除了 `homeSpan.setMaxConnections()`，该函数在很多版本之前就被弃用了**

* **删除了独立的 `SpanRange` 结构，该结构在很多版本之前就已被*弃用*了**
  * 这对特征方法 `setRange()` 的标准使用没有影响
      
有关此更新中包含的所有更改和错误修复的详细信息，请参阅 [发布](https://github.com/HomeSpan/HomeSpan/releases)。

## ❗最新更新 - HomeSpan 1.9.0 (2/17/2024)

* **HomeSpan 已优化，使用更少的 RAM！**

  * 使用相同数量的内存支持大约**两倍**数量的配件
  * 最小化内存使用也意味着用户有更多空间将非 HomeSpan 功能添加到他们的草图中而不会耗尽内存，特别是当非 HomeSpan 代码消耗大量堆栈空间时
  * HomeSpan 现在会自动检测 **PSRAM** (SPIRAM) 的存在，并将最大程度地利用这些额外内存，为某些需要内部 RAM 的 HomeSpan 功能和 ESP32 功能（例如 WiFi）保持内部 RAM 可用。同时为任何不使用（或不能使用）PSRAM 的非 HomeSpan 代码保持内部 RAM 可用
  * 将 HomeSpan 的 41 个配件限制增加到 150 个配件限制（由 HAP 指定），因为现在可以创建具有超过 41 个配件的设备而不会耗尽内存，特别是如果使用 PSRAM

* **HomeSpan 已优化，使用的非易失性存储 (NVS) 显著减少**

  * 允许您使用 NVS 保存更多特性的值
  * 请参阅新添加的 [CustomNVSPartition](docs/Tutorials.md#customnvspartition) 示例，该示例演示了如何创建自己的分区方案，以进一步扩展 NVS 分区的大小（超出 ESP32 默认值），以支持具有大量附件的草图，每个附件都配置为使用 NVS 保存许多特性的值

* **服务和特性的新功能和文档**
  * 为每个适用的特性创建了“枚举常量”（例如 *SWING_ENABLED*、*HUMIDIFYING* 等），可以在读取和写入值时代替整数使用
    * 非常有用，因为 Apple 不再发布其非商业 HAP 文档，该文档提供了每个特性的状态列表和描述
    * 示例：`if(target.getNewVal()==target.ARM_STAY) {...}`
  * 添加了使用新 **Characteristic::ConfiguredName()** 正确命名单个配件中各个服务的能力
    * 有关详细信息，请参阅修订后的 [示例  11 - 服务名称](examples/11-ServiceNames/11-ServiceNames.ino)
  * 新的 [服务和特性](docs/ServiceList.md) 页面现在提供 HomeSpan 支持的每个服务和特性的功能描述和详细规范，包括可用的枚举常量列表对于每个特征
   
* **新增使用 *反转按钮* 和 *触摸传感器* 作为控制按钮的功能**

  * 将 *triggerType* 添加为 `Span& setControlPin(uint8_t pin, triggerType_t triggerType)` 的第二个可选参数
  * 支持 TRIGGER_ON_LOW、TRIGGER_ON_HIGH、TRIGGER_ON_TOUCH 或任何用户定义的函数
  * 请参阅 [API 参考](docs/Reference.md) 了解详情

* **新增通过反复对设备进行电源循环来“远程”触发用户定义操作的功能**

  * 添加新的 homeSpan 方法 `Span& setRebootCallback(void (*func)(uint8_t count), uint32_t upTime=5000)`
  * HomeSpan 传递给 *func* 的参数 *count* 表示在当前重启之前发生的“短”重启次数，其中“短”重启是指在 *upTime* 毫秒过去之前发生的任何重启
  * 可用于远程将不易访问的设备恢复到预定义状态
  * 请参阅 [API 参考](docs/Reference.md) 了解详情

* **添加了两个新的步进电机驱动器**

  * **Stepper_UNIPOLAR**：适用于任何 4 线中心抽头单极电机的通用驱动器
  * **Stepper_ULN2003A**：支持 ULN2003A 驱动板
  * 详情请参阅 [步进电机控制步进器](docs/Stepper.md)

* **附加 Web 日志功能**

  * 添加新的 homeSpan 方法 `Span& setWebLogCallback(void (*func)(String &))`
    * 允许用户在 Web 日志中包含其他数据和自定义 HTML
  * 添加新的 homeSpan 方法 `getWebLog(void (*f)(const char *, void *), void *args)`
    * 允许用户从 sketch 中检索底层 Web 日志 HTML
  * 修改了 `enableWebLog()`，以便它可用于从 NTP 服务器设置时间，而无需实际提供 Web 日志页面
  * 请参阅 [消息日志](docs/Logging.md) 了解详细信息

* **添加了“链接”*homeSpan* 方法的功能**

  * 将之前返回 *void* 的各种 *homeSpan* 方法转换为现在返回 *Span &*
  * 示例：`homeSpan.setControlPin(21).setStatusPin(13);`
  * 请参阅 [API 参考](docs/Reference.md) 了解详情

* **添加了禁用 SpanPoint 加密的功能**

  * 如果不加密，则允许的最大 SpanPoint 设备数量从 7 个增加到 20 个
  * 请参阅 [Span 热点](docs/NOW.md) 了解详情
 
* **此版本中包含的其他新 *homeSpan* 方法：**

  * `Span& setVerboseWifiReconnect()` - 可选择隐藏“尝试连接到...”消息
  * `Span& setWifiCallbackAll()` - 每次连接或重新连接 WiFi 时提供可选回调
  * `TaskHandle_t getAutoPollTask​​()` - 返回 HomeSpan 自动轮询任务的任务句柄

* **删除了对各种“额外” `#include` 文件的依赖**
  * 以下 \#include 文件现在嵌入在 *HomeSpan.h* 中，**不应在任何草图中指定：**
    * *extras/Pixel.h*
    * *extras/RFControl.h*
    * *extras/PwmPin.h*
    * *extras/StepperControl.h*

> [!IMPORTANT]
> 目前可以将上述 `#include` 文件包含在您的草图中（它们对编译的代码没有影响），但它们将来会被删除，因此请立即将它们从您的草图中删除，以确保与后续版本的向前兼容性。
      
有关此更新中包含的所有更改和错误修复的详细信息，请参阅 [发布](https://github.com/HomeSpan/HomeSpan/releases)。


## ❗最新更新 - HomeSpan 1.8.0 (7/8/2023)

* **新的步进电机控制！**

  * 添加新的 StepperControl 类，允许一个或多个步进电机在后台平稳、不间断地运行，同时 HomeSpan 继续在前台同时运行
  * 支持带或不带 PWM 的驱动板，包括微步进模式
  * 支持自动加速和减速，实现平稳启动和停止
  * 可以将电机设置为绝对位置，也可以指示电机移动指定的步数
  * 提供在电机停止后自动进入“制动”状态以节省电量的选项
  * 包括一个完整的电动窗帘示例
  * 请参阅 [步进电机控制](docs/Stepper.md) 了解详情

* **升级 HomeSpan Web 日志输出**

  * 添加新方法 `void homeSpan.setWebLogCSS(const char *css)`，允许您为 Web 日志文本、表格和背景定义自定义样式表 (CSS)
  * 添加 Sodium 和 MbedTLS 库的版本号、HomeKit 配对状态以及重置原因代码的文本描述
  * 请参阅 [消息日志](docs/Logging.md) 了解详情

* **升级到 Web 日志时间服务器初始化**

  * 启动时从 NTP 服务器检索时间和日期的过程现在在后台作为单独的任务运行
  * HomeSpan 在 NTP 查询期间不再被阻止运行

* **添加新方法来禁用 HomeSpan 对 USB 串口端口的使用**

  * 新的日志级别 -1 导致 HomeSpan 抑制所有 OUTPUT 消息
  * 新的 homeSpan 方法 `setSerialInputDisable(boolean val)` 禁用/重新启用 HomeSpan 将 CLI 命令 INPUT 读取到 Arduino 串口监视器的功能

* **添加功能使用非标准 LED 作为 HomeSpan 状态 LED**

  * 新的 homeSpan 方法 `setStatusDevice(Blinkable *sDev)` 将状态 LED 设置为 Blinkable 对象 sDev
  * 允许将连接到引脚扩展器的 LED 或任何其他非标准 LED 控制器（例如，当引脚为低电平而不是高电平时亮起的反向 LED）用作 HomeSpan 状态 LED
  * 有关如何创建 Blinkable 对象的详细信息（包括示例），请参阅 [Blinkable.md](https://github.com/HomeSpan/HomeSpan/blob/release-1.8.0/docs/Blinkable.md)

有关此更新中包含的所有更改和错误修复的详细信息，请参阅 [发布](https://github.com/HomeSpan/HomeSpan/releases)。


## ❗最新更新 - HomeSpan 1.7.2 (4/5/2023)

* **新增在 Sketch 中设置 OTA 密码的功能**
  * 详情请参阅 [OTA 页面](docs/OTA.md)

* **添加了逻辑，允许在草图中的多个文件中“定义”相同自定义特征的副本**

  * 允许在同一草图中的多个文件中使用相同的自定义特征，而无需编译器抛出“重新定义错误”
  * 请参阅 [API 参考](docs/Reference.md) 了解详情

* **setValidValues() 的扩展功能不仅可以用于 UINT8 特征**

  * 现在可以使用 INT、UINT16 和 UINT32 特征以及 UINT8 特征
  * 请参阅 [API 参考](docs/Reference.md) 了解详情

* **在 autoPoll() 中添加了新参数，允许用户设置优先级并选择 CPU**

  * 提供双处理器芯片的增强性能
  * 请参阅 [API 参考](docs/Reference.md) 了解详情

* **自动 LED 渐变！**

  * 为 LedPin 类添加了新方法，可使用 ESP32 的内置渐变控制
  * 允许用户指定渐变速度
  * 在后台运行，不消耗任何 CPU 资源
  * 请参阅 [PWM 页面](docs/PWM.md) 了解详情

* **添加了将配对数据从一个设备克隆到另一个设备的功能**

  * 向 CLI 添加新的 "P" 和 "C" 命令
  * 允许将损坏的设备换成新设备（运行相同的草图），而无需取消旧设备的配对或配对新设备
  * 避免丢失与设备相关的自动化、场景和任何其他“家庭”应用自定义
  * 新旧设备可以是不同的芯片（例如 ESP32-S2 与 ESP32-C3）
  * 请参阅新的 [克隆页面](docs/Cloning.md) 了解详情

有关此更新中包含的所有更改和错误修复的详细信息，请参阅 [发布](https://github.com/HomeSpan/HomeSpan/releases)。


## ❗最新更新 - HomeSpan 1.7.1 (2/4/2023)

* **SpanPoint 支持 ESP8266！**

  * 在 ESP8266 上使用 ESP-NOW 连接到在 ESP32 上运行的 HomeSpan
  * 查看 [Span 热点](docs/NOW.md) 了解更多信息以及详细的 ESP8266 示例

* **SpanPoint WiFi 信道扫描升级**

  * SpanPoint 现在将成功用于传输的最后一个 WiFi 信道保存在非易失性存储中
  * 避免了 SpanPoint 在重新启动时需要重新启动对所有信道的全面扫描
  * 与深度睡眠功能结合使用时可显着延长电池寿命

* **新的 SpanToggle 类**

  * 类似于 SpanButton，但专为拨动开关而设计
  * 集成的去抖动逻辑可防止误触发
  * 非常适合与接触传感器一起使用
  * 有关详细信息，请参阅 [API 参考](docs/Reference.md)

  * **添加了电视扬声器服务**
 
  * 通过苹果的远程应用控制电视音量
  * 请参阅 [电视服务](docs/TVServices.md) 页面了解详情和示例

* **添加了对字节数组（"DATA"）特性的支持**

  * 可用于试验其他 HomeKit 应用程序，例如 Eve 应用
  * 包括三种新的特性方法：`setData()`、`getData()` 和 `getNewData()`
  * 包括新的宏 `CUSTOM_CHAR_DATA()`，可轻松创建自定义字节数组特性
  * 请参阅 [API 参考](docs/Reference.md) 了解详情

* **LedPin 升级**

  * 反转 PWM 信号的新选项
  * 可用于生成两个独立的异相 PWM 信号（一个反转，一个不反转），以推拉方式驱动某些双针设备，例如压电蜂鸣器
  *请参阅 [PWM 页面](docs/PWM.md) 了解详情

* **错误修复**

  * 添加了逻辑以防止 Access 的额外广播使用 SpanPoint 时指向 SSID

有关此更新中包含的所有更改和错误修复的详细信息，请参阅 [发布](https://github.com/HomeSpan/HomeSpan/releases)。


## ❗最新更新 - HomeSpan 1.7.0 (11/11/2022)

* ESP-NOW 现已完全集成到 HomeSpan！

  * 新的专用类 SpanPoint，可促进多个 ESP32 设备之间的双向设备到设备通信
  * 提供 WiFi 信道的自动校准，以确保与 HomeSpan 的正常 WiFi 连接兼容
  * 包括详细的示例草图，演示如何使用 SpanPoint 实现电池供电的远程传感器
  * 有关完整详细信息，请参阅专用的 [Span 热点](docs/NOW.md) 教程页面
* NeoPixels 现在可以用作状态 LED

  * 添加 `homeSpan.setStatusPixel()` 方法
  * 与具有内置 NeoPixel LED 的 ESP32 板配合使用
  * 有关详细信息，请参阅 [API 参考](docs/Reference.md) 
* 用于跟踪 HomeSpan 运行时状态的新功能

  * 添加 `homeSpan.setStatusCallback()` 方法，每当 HomeSpan 的内部状态发生变化时，例如从 WiFi 需要变为 WiFi 连接时，都会为用户提供回调函数...
  * 跟踪 HomeSpan 运行时状态的变化，这些变化通常会触发（可选）状态 LED 闪烁模式的变化
  * 有关详细信息，请参阅 [API 参考](docs/Reference.md) 
* 重要错误修复

  * 修复了控制器更新逻辑中的错误，该错误与 Apple 现在处理 HomeKit Hub 的方式的更改有关，该错误会产生“错误：设备尚未配对！”消息

  * 修复了触摸传感器逻辑中的错误，该错误在使用 Arduino-ESP32 版本 2.0.0-2.0.2 时会导致编译失败

有关此更新中包含的所有更改和错误修复的详细信息，请参阅 [发布](https://github.com/HomeSpan/HomeSpan/releases)。


## ❗最新更新 - HomeSpan 1.6.0 (8/20/2022)
* 支持 ESP32-S3 设备

  * 需要 Arduino-ESP32 Board Manager 2.0.4 或更高版本
* 新功能可在不重启设备的情况下动态添加/删除附件

  * 添加 `homeSpan.deleteAccessory()` 和 `homeSpan.updateDatabase()` 方法
  * 包括新的  [示例 20 - 高级技术](examples/20-AdvancedTechniques/20-AdvancedTechniques.ino)，以演示如何使用这些方法创建动态桥接
  * 有关详细信息，请参阅 [API 参考](docs/Reference.md) 
* 对触摸板的新支持

  * `SpanButton()` 现在支持三种引脚触发方法：`TRIGGER_ON_LOW`、`TRIGGER_ON_HIGH` 和 `TRIGGER_ON_TOUCH`
  * 还允许用户添加自己的触发方法，以便 `SpanButton()` 可以监控连接到引脚扩展器、引脚多路复用器或任何其他需要调用第三方库函数的设备上的按钮
  * 有关详细信息，请参阅 [API 参考](docs/Reference.md) 
* 改进了 WiFi 断开/重新连接处理

  * 修复了在某些情况下可能阻止 HomeSpan 的代码信号丢失后重新连接到 WiFi
  * 将 WiFi 诊断添加到 Web 日志以监控断开连接和 WiFi 信号强度
* 新选项可将 HomeSpan 作为其自身线程中的单独任务运行

  * 添加 `homeSpan.autoPoll()` 方法
  * 适用于单核和双核处理器
  * 对于双核处理器，将在未用于其他 Arduino 功能的“空闲”核心上创建轮询任务
  * 允许用户将时间敏感代码添加到主 Arduino `loop()` 函数中，而不会延迟或被 HomeSpan 轮询拖延
  * 有关详细信息，请参阅 [API 参考](docs/Reference.md) 
* 新的可编程集线器示例

  * 演示如何实现 Web 界面，该界面允许用户在桥接设备上动态添加、删除和配置最多 12 个单独的灯具配件
  * 扩展了 [示例 20 - 高级技术](examples/20-AdvancedTechniques/20-AdvancedTechniques.ino) 中使用的许多方法
* 其他更新包括：

  * 新的 "m" CLI 命令，用于打印堆内存中剩余的空闲字节——可用于在动态添加配件时监控内存使用情况
  * 新的 `homeSpan.getLogLevel()` 返回当前日志级别的方法

有关此更新中包含的所有更改和错误修复的详细信息，请参阅 [发布](https://github.com/HomeSpan/HomeSpan/releases)。


## ❗最新更新 - HomeSpan 1.5.1 (4/17/2022)

* 新的 Web 日志功能

  * HomeSpan 现在可以托管用于消息日志记录的 Web 日志页面
  * 新的 `WEBLOG()` 宏使创建用户定义的日志消息变得容易
  * 提供可选的 NTP 时间服务器来设置设备时钟，以便所有消息都可以正确加盖时间戳
  * 有关完整详细信息，请参阅 [消息记录](docs/Logging.md)
* `LOG()` 宏的新 `printf` 样式格式

  * 添加 `LOG0()`、`LOG1()` 和 `LOG2()` 宏的可变形式，以便它们可以以与标准 C printf 函数相同的方式使用
  * 大大简化了日志消息的创建
  * 有关完整详细信息，请参阅 [消息记录](docs/Logging.md)
* 新的 `CUSTOM_SERV()` 宏

  * 允许创建自定义服务
  * 可以与现有的 `CUSTOM_CHAR()` 宏结合使用，以生成 HAP-R2 中提供的服务以外的服务
  * 包括一个完整的自定义压力传感器配件示例，该配件可被 Eve 应用识别
  * 有关详细信息，请参阅 [API 参考](docs/Reference.md) 
* 新OTA 更新的“安全加载”模式

  * HomeSpan 可以检查以确保通过 OTA 上传的新草图是另一个 HomeSpan 草图。如果不是，则上传失败
  * 在 OTA 更新后重新启动时，HomeSpan 会检查以确保在更新的草图中启用了 OTA。如果没有，HomeSpan 会回滚到草图的先前版本
  * 有关完整详细信息，请参阅 [OTA](docs/OTA.md)
* 其他更新包括：

  * `SpanUserCommand()` 的新（可选）参数，允许将指针传递给任意数据结构
  * 新的 `SPAN_ACCESSORY()` 宏，可扩展为创建附件时经常使用的常用代码片段
  * 更新和简化的示例教程，并完全重新设计示例 7 和 11，以最好地符合 iOS 15.4 下的“家庭”应用行为

有关此更新中包含的所有更改和错误修复的详细信息，请参阅 [发布](https://github.com/HomeSpan/HomeSpan/releases)。


## ❗最新更新 - HomeSpan 1.5.0 (2/20/2022)

* 新的集成库用于控制单线和双线可寻址 RGB 和 RGBW LED 及 LED 灯带！

  * 添加了两个新类：

    * `Pixel()` 用于控制单线 RGB 和 RGBW LED 及 LED 灯带，例如此 NeoPixel RGBW LED
    * `Dot()` 用于控制双线 RGB LED 和 LED 灯带，例如此 DotStar RGB 灯带
  * 有关完整详细信息，请参阅 [灯带](docs/Pixels.md)，其中包括详细的教程草图，演示了使用 `Pixel()` 和 `Dot()` 类的不同方式，以及高级 HomeSpan "HolidayLights" 项目，展示了如何开发自定义特效！
"
* 将同时连接的控制器最大数量从 8 个增加到 14 个（适用于 Arduino-ESP32 2.0.1 版及更高版本）

  * 向全局 homeSpan 对象添加了新方法 `reserveSocketConnection(uint8_t nSockets)`，以便更好地管理自定义连接

  * 弃用旧版 `setMaxConnections(uint8_t nCon)` 方法
* 其他更新包括：

  * 向 SpanCharacteristic 添加了新方法 `setDescription(const char *desc)` 和 `setUnit(const char *unit)`。在创建和使用自定义特性时很有用
  * 向全局 homeSpan 对象添加了新方法 `setStatusAutoOff(uint16_t duration)`。使状态 LED（如果使用）在 duration 秒后自动关闭。对于位于卧室或电视室的设备非常方便！
  * 向全局 homeSpan 对象添加了新方法 `setPairCallback(func)`。允许您在 HomeSpan 将设备与“家庭”应用配对或随后取消配对时创建自定义操作
  * 向全局 homeSpan 对象添加了新方法 `deleteStoredValues()`。提供一种编程方式来删除存储在 NVS 中的所有特性的值设置

有关此更新中包含的所有更改和错误修复的详细信息，请参阅 [发布](https://github.com/HomeSpan/HomeSpan/releases)。

## ❗最新更新 - HomeSpan 1.4.2 (11/27/2021)

* 更新了与​​ Arduino-ESP32 Board Manager 2.0.1 的兼容性

  * 保持与所有以前版本的向后兼容性
* 一些用于提前使用情况的新方法和选项：

  * 为 `setVal()` 方法添加了可选的第二个参数，允许在不向 HomeKit 发送通知消息的情况下更新特性的值。在实施喷水灭火系统时，可用于跟踪持续时间 - 请参阅 HomeSpan Reference Sprinkler 以获取示例

  * 将 `getLinks()` 添加为 SpanService 的新方法。返回指向已使用 `addLink()` 方法链接到另一个服务的 SpanServices 的指针向量。可用于循环遍历所有链接的服务，例如检查淋浴系统中的所有阀门 - 请参阅 HomeSpan Reference Shower 以获取示例

  * 将 `setPerms()`、`addPerms()` 和 `removePerms()` 添加为 SpanCharacteristic 的新方法。允许用户修改（设置/添加/删除）任何特性的默认权限。可用于添加/删除某些特性的写入权限 - 请参阅 [电视服务](docs/TVServices.md) 

  * 向全局 homeSpan 对象添加了 `setPairingCode()` 方法，允许以编程方式配置草图内的配对设置代码。请参阅 [API 参考](docs/Reference.md) 了解使用此功能时的重要安全注意事项！

有关此更新中包含的所有更改和错误修复的详细信息，请参阅 [发布](https://github.com/HomeSpan/HomeSpan/releases)。

## ❗最新更新 - HomeSpan 1.4.1 (10/31/2021)

* 电视服务和特性已添加到 HomeSpan！有关完整详情，请参阅 [电视服务](docs/TVServices.md)
* RFControl 库已更新，允许生成适合控制红外 LED 的调制载波。这允许您使用 HomeSpan 创建支持 HomeKit 的电视遥控器。请参阅 [HomeSpan 项目](https://github.com/topics/homespan)，了解一些实际示例！
* 可以使用新宏将用户定义的自定义特性添加到 HomeSpan。有关详细信息，请参阅 [HomeSpan API](docs/Reference.md)（仅限高级用户）

有关此更新中包含的所有更改和错误修复的详细信息，请参阅 [发布](https://github.com/HomeSpan/HomeSpan/releases)。

## ❗最新更新 - HomeSpan 1.4.0 (10/9/2021)

HomeSpan 现在与 Arduino-ESP32 Board Manager 2.0.0 版完全兼容，并将在以下乐鑫芯片上运行：

* ESP32
* ESP32-S2
* ESP32-C3

如果您需要恢复到 Arduino-ESP Board Manager 1.0.6 版，HomeSpan 还保持与该版本完全向后兼容。这是一个复杂的更新！请报告使用 Arduino-ESP32 2 版时发现的任何错误或问题。
HomeSpan 1.4.0 还包含以下新功能和增强功能：

* PWM 库已升级

  * 用户可以为每个 `LedPin()` 实例指定自定义 PWM 频率（受芯片特定资源限制）
  * 用户可以将 `LedPin()` 的占空比设置为十进制浮点数（例如 34.56），而不仅仅是整数
  * 库会自动将占空比分辨率设置为所选 PWM 频率的最大允许值
  * 库优化多个 `LedPin()` 和 `ServoPin()` 实例的分布，以确保使用所有可用的 PWM 通道
* RFControl 库已升级

  * 库允许传输任意长度的脉冲串
  * 用户可以将脉冲串预加载到一个或多个任意长度的 32 位字数组中，以便根据需要进行按需传输

* 用户现在可以使用新方法 `setValidValues()` 限制“家庭”应用中某些特征（例如安全系统的目标状态）的选择

* 状态 LED 和控制按钮现在是可选组件，除非特别启用，否则将被忽略
  * 由于 HomeSpan 现在在具有许多不同引脚配置的芯片上运行，因此 HomeSpan 对状态 LED 和控制按钮使用的预设引脚号可能会与许多设备发生冲突
  * HomeSpan 的默认行为已更改为忽略与状态 LED 和控制按钮相关的所有逻辑
  * 要启用状态 LED，您必须使用常用方法 `homeSpan.setStatusPin(pin)` 指定 LED 连接的引脚
  * 要启用控制按钮，您必须使用常用方法 `homeSpan.setControlPin(pin)` 指定控制按钮连接的引脚

有关此更新中包含的所有更改和错误修复的详细信息，请参阅 [发布](https://github.com/HomeSpan/HomeSpan/releases)。


## 最新更新 - HomeSpan 1.3.0 (6/20/2021)

此更新带来了许多新功能和增强功能：

* PWM 库已升级，
  * 可更轻松地控制多达 16 个可调光 LED，并且
  * 扩展了专用类，可同时操作多达 8 个伺服电机！
* 特性值可以自动保存在非易失性存储器中，以便在断电时保留。当电源恢复时，您的配件将自动恢复到其最新状态！
* HomeSpan CLI 现在可以自定义 - 使用您自己的功能和命令扩展 CLI！
* 在启动时，只要未找到 WiFi 凭据，即可自动启动 HomeSpan 的 WiFi 接入点。
* 对于高级用户：创建您自己的自定义 WiFi 接入点并以编程方式设置您的 WiFi 凭据。

有关此更新中包含的所有更改和错误修复的详细信息，请参阅 [发布](https://github.com/HomeSpan/HomeSpan/releases)。

## 最新更新 (3/13/2021)

* HomeSpan 1.2.1 - 此更新为所有特性添加了运行时范围检查，如果您为特性设置的初始值或对该值进行的任何后续更改超出特性允许的最小/最大范围，则会向您发出警告。这有助于诊断 Home 应用中的“无响应”错误。此更新还引入了 `setRange(min,max,step)`，作为一种用于更改特性范围的全新且更强大的方法。

有关此更新中包含的所有更改和错误修复的详细信息，请参阅 [发布](https://github.com/HomeSpan/HomeSpan/releases)。

## 最新更新 (2/18/2021)

* HomeSpan 1.2.0 - HomeSpan 现在支持直接从 Arduino IDE 进行无线 [OTA](docs/OTA.md) 更新（无需串口连接）！此版本还增加了对链接服务的支持，并包含一个新的教程 [示例 7 - 附件名称](examples/07-AccessoryNames/07-AccessoryNames.ino)，演示如何使用链接服务实现多头水淋浴。其他新功能包括设置草图版本号的能力（在使用 OTA 更新时很有用），以及一种在建立 WiFi 连接后指定要回调的用户定义函数的方法。

有关此更新中包含的所有更改和错误修复的详细信息，请参阅 [发布](https://github.com/HomeSpan/HomeSpan/releases)。
## 最新更新 (1/24/2021)

* HomeSpan 1.1.4 - HomeSpan 现在可以识别二维码进行配对！此版本还包含一种新方法，允许您更改用于与 HomeKit 通信的 TCP 端口，以及一种修改主机名后缀（或将其全部消除）的新方法。改进了通过串口监视器地址解析文本输入的功能（希望如此！），解决了 HomeSpan 与 Platform IO 结合使用时出现的问题。

有关此更新中包含的所有更改和错误修复的详细信息，请参阅 [发布](https://github.com/HomeSpan/HomeSpan/releases)。
