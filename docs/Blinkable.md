<!--   原文时间：2023.7.3,校对时间：2025.8.12  -->

# 使用 Blinkable 接口创建自定义状态 LED

HomeSpan 的状态 LED 会通过不同的闪烁模式向用户传达 HomeSpan 的运行状态信息。  
*HomeSpan* 的 `setStatusPin()` 和 `setStatusPixel()` 方法分别允许你将状态 LED 设为普通 LED 或 NeoPixel LED。  
然而，状态 LED 也可以设置为**任何实现了 Blinkable 接口**[^1] 的对象，只需通过 `homeSpan` 方法 `setStatusDevice(Blinkable *sDev)` 指定，其中 `sDev` 即为一个 Blinkable 对象。

要创建你自己的 Blinkable 对象，首先需要新建一个继承自 **Blinkable** 的子类。  
接着，添加一个构造函数，用于定义引脚并在需要时执行初始化操作。  
最后，实现以下**必须**的方法，这些方法会在 **Blinkable** 控制设备闪烁时被调用：

* `void on()` - 打开设备（例如点亮 LED）
* `void off()` - 关闭设备（例如熄灭 LED）
* `int getPin()` - 返回设备的引脚编号（任意数字均可，不必是真正的 ESP32 引脚号）

例如，下面的代码定义了一个 Blinkable 对象，用于驱动**反向 LED**（当 ESP32 引脚为 LOW 时点亮，为 HIGH 时熄灭）：

```C++
// 在代码顶部附近创建如下结构体

struct invertedLED : Blinkable {        // 创建继承自 Blinkable 的子类

  int pin;                              // 存储引脚编号的变量

  invertedLED(int pin) : pin{pin} {     // 构造函数，初始化引脚
    pinMode(pin,OUTPUT);                // 将引脚设置为输出模式
    digitalWrite(pin,HIGH);             // 先置高电平（对于反向 LED 来说表示熄灭）
  }

  void on() override { digitalWrite(pin,LOW); }        // 必需的 on() 方法-置低电平点亮
  void off() override { digitalWrite(pin,HIGH); }      // 必需的 off() 方法-置高电平熄灭
  int getPin() override { return(pin); }               // 必需的 getPin() 方法-返回引脚编号
};

...

// 然后在 setup() 中使用该结构体来设置状态 LED

void setup(){

  homeSpan.setStatusDevice(new invertedLED(13));    // 将状态 LED 设置为连接在引脚 13 的新 Blinkable 设备

...
}
```

[^1]: 在 C++ 中，*接口*是仅包含纯虚函数的任何抽象类。你无法实例化接口，但可以实例化该接口的任何派生子类，前提是你定义了每个所需的虚拟函数。

---

[↩️](Reference.md) 返回到API页面
