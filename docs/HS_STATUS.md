# HomeSpan 状态和状态指示灯

除了跟踪您在 HomeSpan 程序中实现的所有 HomeKit 配件、服务和特性之外，HomeSpan 还会跟踪其全局运行状态（例如，是否连接到 WiFi、是否通过 OTA 更新等）。当前的 HomeSpan 状态以枚举类型 **HS_STATUS** 表示，可以直接在您的程序中读取，也可以通过设备上可选的状态指示灯的不同闪烁模式进行可视化显示。该指示灯可以是简单的单色模拟 LED，也可以是可寻址像素 LED。许多制造商在其 ESP32 开发板上集成了其中一种或两种 LED，当然您也可以根据需要使用单独的 LED。

要启用 HomeSpan 的状态指示灯功能，只需在 HomeSpan 程序的顶部附近调用 `homeSpan.setStatusPin()` 或任何相关函数（具体取决于您使用的 LED 类型）。有关启用不同类型状态指示灯的各种选项的详细信息，请参阅[参考 API](Reference.md)。

要从程序内部读取状态，请调用 `homeSpan.getStatus()`。此函数返回一个 `std::pair<HS_STATUS, uint32_t>` 形式的值对，其中第一个元素表示 HomeSpan 的当前状态，第二个元素表示 HomeSpan 进入该状态以来的持续时间（以秒为单位）。每当 HomeSpan 状态改变时，此持续时间都会重置为零，也可以通过调用 `homeSpan.resetStatusDuration()` 手动重置。

例如，将以下代码添加到程序的主循环 `loop()` 中，即可监控 HomeSpan 的状态，并在设备尚未配对时每 30 秒输出一次警告：

```C++
void loop(){

  homeSpan.poll();

  auto [ status, duration] = homeSpan.getStatus();        // 使用“auto”可以轻松地从返回 std::pair 的函数中读取数据
  if(status==HS_PAIRING_NEEDED && duration > 30){         // 检查当前状态是否已持续超过 30 秒
    Serial.printf("Warning: HomeSpan is not paired.\n");  // 打印警告信息
    homeSpan.resetStatusDuration();                       // 重新启动状态持续时间计时器
  }  
}
```

除了直接读取 HomeSpan 的状态之外，还可以使用 `homeSpan.setStatusCallback(void (*func)(HS_STATUS status))` 方法创建一个可选的回调函数 `*func*`，HomeSpan 会在状态改变时调用该函数。[^setup] HomeSpan 会向 `*func*` 传递一个类型为 `**HS_STATUS**` 的参数 `*status*`，您可以在回调函数中读取该参数。为了方便使用，HomeSpan 还提供了一个便捷函数 `const char* homeSpan.statusString(HS_STATUS s)`，该函数会为每个 `**HS_STATUS**` 类型返回一个预定义的字符串。

[^setup]：请注意，HomeSpan 启动时状态初始化为 `*HS_INITIAL_SETUP*`，该状态本身不会触发回调。回调只有在 HomeSpan 首次将其初始状态从 `*HS_INITIAL_SETUP*` 更改为其他值时才会触发，之后每次状态更改都会触发回调。

例如，以下代码创建了一个可选的回调函数，用于在 HomeSpan 状态发生变化时打印该状态：

```C++
#include "HomeSpan.h"

void setup(){
  homeSpan.setStatusCallback(statusUpdate);   // 设置回调函数
  ...
  homeSpan.begin();
  ...
}

// 创建一个回调函数，当 HomeSpan 状态发生变化时，该函数只需在串口监视器上打印预定义的字符串即可。

void statusUpdate(HS_STATUS status){
  Serial.printf("\n*** HOMESPAN STATUS CHANGE: %s\n",homeSpan.statusString(status));
}
```

当然，您可以在 *func* 中创建任何其他消息或执行任何所需的操作，而无需使用 HomeSpan 预定义的字符串。请注意，HomeSpan 回调函数还可以接受匿名 *lambda 函数* 作为参数。例如，以下一行代码将创建一个回调函数，当 HomeSpan 状态发生变化时，该函数会将红色格式的日志消息输出到 Web 日志：

```C++
void setup(){
  ...
  homeSpan.setStatusCallback([](HS_STATUS status){WEBLOG("<span style\=\"color:red\">HOMESPAN STATUS: %s</span>",homeSpan.statusString(status));});
  ...
}
```

### HomeSpan 状态类型表

下表列出了所有可能的 HomeSpan **HS_STATUS** 状态类型，以及每种类型的预定义字符串（由 `statusString()` 返回）和 HomeSpan 状态指示灯（如果启用）上该状态类型对应的闪烁模式的图形表示。点击任意条目可查看更多详情。请注意，所有图形表示均已缩放，仅显示每种模式的前 6 秒，这足以清晰地显示模式（闪烁次数、持续时间等）。

|HomeSpan 状态 (HS_STATUS)|状态字符串|状态指示灯图案|
|---|---|---|
|<details><summary>HS_INITIAL_SETUP</summary><i>HomeSpan 正在运行草图 setup() 部分的代码</details>|HomeSpan 初始化中|<img src="images/ledPatterns/off.svg" width=300>|

|<details><summary>HS_WIFI_NEEDED</summary><i>WiFi 凭据尚未设置/存储，且以太网接口不可用</details>|需要 WiFi 凭据|<img src="images/ledPatterns/slowSingleBlink.svg" width=300>|

|<details><summary>HS_WIFI_SCANNING</summary><i>HomeSpan 正在扫描（或重新扫描）WiFi 网络接入点</details>|WiFi 扫描已开始|<img src="images/ledPatterns/longTripleBlink.svg" width=300>|

|<details><summary>HS_WIFI_CONNECTING</summary><i>HomeSpan 正在尝试连接到已存储的 WiFi 凭据中指定的 WiFi 网络</details>|WiFi 正在连接|<img src="images/ledPatterns/slowFlashing.svg" width=300>|

|<details><summary>HS_ETH_CONNECTING</summary><i>HomeSpan 正在尝试使用已配置的以太网接口连接到以太网</details>|以太网连接中|<img src="images/ledPatterns/slowFlashing.svg" width=300>|

|<details><summary>HS_PAIRING_NEEDED</summary><i>HomeSpan 已连接到网络，但设备尚未与 HomeKit 配对</details>|设备尚未配对|<img src="images/ledPatterns/slowDoubleBlink.svg" width=300>|

|<details><summary>HS_PAIRED</summary><i>HomeSpan 已连接到网络，并且已与 HomeKit 配对，但没有活动的 HomeKit 连接</details>|已配对并等待 HomeKit|<img src="images/ledPatterns/slowDoubleBlinkInverted.svg" width=300>|

|<details><summary>HS_CONNECTED</summary><i>HomeSpan 已连接到网络，设备已与 HomeKit 配对，并且至少有一个活动的 HomeKit 连接</details>|设备已连接到 HomeKit|<img src="images/ledPatterns/on.svg" width=300>|

|<details><summary>HS_ENTERING_CONFIG_MODE</summary><i>用户已请求设备进入命令模式</details>|进入命令模式|<img src="images/ledPatterns/rapidFlashing.svg" width=300>|

|<details><summary>HS_CONFIG_MODE_EXIT</summary><i>HomeSpan 处于命令模式，且已指定“退出命令模式”作为选项</details>|1. 退出命令模式|<img src="images/ledPatterns/fastBlink1.svg" width=300>|

|<details><summary>HS_CONFIG_MODE_REBOOT</summary><i>HomeSpan 处于命令模式，且已指定“重启”作为选项</details>|2.重启设备|<img src="images/ledPatterns/fastBlink2.svg" width=300>|

|<details><summary>HS_CONFIG_MODE_LAUNCH_AP</summary><i>HomeSpan 处于命令模式，已指定“启动接入点”作为选项</details>|3. 启动接入点|<img src="images/ledPatterns/fastBlink3.svg" width=300>|

|<details><summary>HS_CONFIG_MODE_UNPAIR</summary><i>HomeSpan 处于命令模式，已指定“取消配对设备”作为选项</details>|4. 取消配对设备|<img src="images/ledPatterns/fastBlink4.svg" width=300>|

|<details><summary>HS_CONFIG_MODE_ERASE_WIFI</summary><i>HomeSpan 处于命令模式，已选择“擦除 WiFi 凭据”</details>|5. 擦除 WiFi 凭据|<img src="images/ledPatterns/fastBlink5.svg" width=300>|

|<details><summary>HS_CONFIG_MODE_EXIT_SELECTED</summary><i>用户已选择“退出命令模式”</details>|正在退出命令模式...|<img src="images/ledPatterns/rapidFlashing.svg" width=300>|
|<details><summary>HS_CONFIG_MODE_REBOOT_SELECTED</summary><i>User has select "Reboot" from the Command Mode</details>|Rebooting Device...|<img src="images/ledPatterns/rapidFlashing.svg" width=300>|
|<details><summary>HS_CONFIG_MODE_LAUNCH_AP_SELECTED</summary><i>User has selected "Launch AP Access" from the Command Mode</details>|Launching Access Point...|<img src="images/ledPatterns/rapidFlashing.svg" width=300>|
|<details><summary>HS_CONFIG_MODE_UNPAIR_SELECTED</summary><i>User has selected "Unpair Device" from the Command Mode</details>|Unpairing Device...|<img src="images/ledPatterns/rapidFlashing.svg" width=300>|
|<details><summary>HS_CONFIG_MODE_ERASE_WIFI_SELECTED</summary><i>User has selected "Erase WiFi Credentials" from the Command Mode</details>|Erasing WiFi Credentials...|<img src="images/ledPatterns/rapidFlashing.svg" width=300>|
|<details><summary>HS_REBOOTING</summary><i>HomeSpan is in the process of rebooting the device</details>|REBOOTING|<img src="images/ledPatterns/off.svg" width=300>|
|<details><summary>HS_FACTORY_RESET</summary><i>HomeSpan is in the process of performing a Factory Reset of device</details>|Performing Factory Reset...|<img src="images/ledPatterns/off.svg" width=300>|
|<details><summary>HS_AP_STARTED</summary><i>The HomeSpan Setup Access Point is started but no client has yet connected</details>|Access Point Started|<img src="images/ledPatterns/rapidDoubleBlink.svg" width=300>|
|<details><summary>HS_AP_CONNECTED</summary><i>The HomeSpan Setup Access Point is running and a client is connected</details>|Access Point Connected|<img src="images/ledPatterns/mediumDoubleBlink.svg" width=300>|
|<details><summary>HS_AP_TERMINATED</summary><i>The HomeSpan Setup Access Point has been terminated</details>|Access Point Terminated|<img src="images/ledPatterns/rapidFlashing.svg" width=300>|
|<details><summary>HS_OTA_STARTED</summary><i>HomeSpan is in the process of receiving an Over-the-Air software update</details>|OTA Update Started|<img src="images/ledPatterns/mediumTripleBlink.svg" width=300>|

### 高级使用场景

* 如果您已将文本显示屏连接到 HomeSpan 设备，请在上述回调函数中添加代码，使其向用户显示 HomeSpan 状态消息。
* 除了启用 HomeSpan 状态 LED 指示灯外，您还可以通过在回调函数中添加自定义逻辑来实现自定义 LED 指示灯模式、使用不同颜色等。

### 重启以解决家庭应用“无响应”问题

当 HomeSpan 设备通过 Wi-Fi 或以太网连接到您的网络后，其状态将变为 *HS_PAIRING_NEEDED* 或 *HS_PAIRED*，具体取决于它是否已与 HomeKit 配对。如果状态为 *HS_PAIRING_NEEDED*，则该状态将保持，直到您使用 iPhone 上的“家庭”App 将设备与 HomeKit 配对。

如果配对已完成，则您的一个或多个 HomeKit 中枢应会自动检测到 HomeSpan 设备并发起安全连接请求。这可能在设备首次连接到您的网络后几秒钟内发生，也可能需要一两分钟。成功验证 HomeSpan 收到的来自 HomeKit 的第一个安全连接请求后，HomeSpan 的状态将变为 *HS_CONNECTED*。在此状态下，您应该能够通过“家庭”App 完全控制您的设备。

如果由于任何原因，HomeKit 关闭了与设备的所有安全连接，但设备仍连接到您的网络并与 HomeKit 配对，HomeSpan 会将其状态重置为 *HS_PAIRED*。然后，它会等待 HomeKit 请求新的安全连接。

因此，HomeSpan 应该只是暂时保持 *HS_PAIRED* 状态。长时间保持 *HS_PAIRED* 状态是不理想的，并且很可能会导致“家庭”App 显示您的设备“无响应”。HomeSpan 本身无法解决此问题，因为 HomeKit 不允许设备主动发起与 HomeKit Hub 的连接。相反，HomeKit 必须主动发起与家庭网络中每个设备的新连接。

以下代码提供了一种可能的解决方案，通过监控 HomeSpan 的状态并检查其是否长时间处于 *HS_PAIRED* 状态。如果出现这种情况，它会认为出了问题并重启自身，希望重启并重新连接到网络后，HomeKit 能够重新与设备建立安全连接：

```C++
void loop(){

  homeSpan.poll();

  auto [ status, duration] = homeSpan.getStatus();        // 使用“auto”可以轻松地从返回 std::pair 的函数中读取数据
  if(status==HS_PAIRED && duration > 120)                 // 检查 HomeSpan 是否已等待 HomeKit 连接超过 2 分钟
    homeSpan.processSerialCommand("R");                   // 如果是，则强制 HomeSpan 重启
}
```

---

[↩️](../README.md#resources) 返回欢迎页面
