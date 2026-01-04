个人翻译的 [HomeSpan 2.1.7](https://github.com/HomeSpan/HomeSpan)（家庭扩展），请前往原文 [下载](https://github.com/HomeSpan/HomeSpan/releases) 体验。

# 欢迎！

欢迎使用 HomeSpan - 一个强大且极其易于使用的 Arduino 库，完全在 [Arduino IDE](http://www.arduino.cc) 内创建你自己的基于 [ESP32](https://www.espressif.com/en/products/modules/esp32) 的 HomeKit 设备。

HomeSpan 实现了以微控制器为中心的 Apple HomeKit 附件协议规范版本 R2 ([HAP-R2](../master/docs/HAP-R2.pdf))，专为在 Arduino IDE 中运行的乐鑫 ESP32 微控制器设计。HomeSpan 通过家庭无线网络与 HomeKit 配对，无需任何外部网桥。通过 HomeSpan，你可以使用 ESP32 I/O 的全部功能来创建自定义控制软件和/或硬件，并能以 iPhone、iPad 或 Mac 上的“家庭”应用或 Siri 自动操作外部设备。

### 要求

|组件 | 要求 | 参见注释 |
|---|:---:|:---:|
|当前 HomeSpan 生产版本 | **2.1.7** | - |
|支持的芯片 | **ESP32、S2、S3、C3 、C5和 C6** | [^8266] |
|最低要求 [Arduino-ESP32 核心](https://github.com/espressif/arduino-esp32) | **3.3.0** | [^fail] |
|最新核心已使用 HomeSpan 进行全面测试 | **3.3.5** | [^tested] |
|最小 Flash 分区大小 | **1.9MB** | - |
|推荐分区方案 | **最小 SPIFFS（1.9MB APP，带 OTA）** | [^partition] |
| HomeKit 中心 | **HomePod 或 Apple TV** | [^homehub] |

[^8266]:HomeSpan 无法在 ESP-8266 上运行，但 ESP-8266 可以与 HomeSpan 的 SpanPoint 功能配合使用，用作远程传感器。

[^fail]: HomeSpan 的当前正式版无法在任何低于最低要求版本的 Arduino-ESP32 核心版本下编译。不过，HomeSpan 的早期版本可以在更早版本的 Arduino-ESP32 核心版本下编译和运行。每个版本的 [HomeSpan 分支](https://github.com/HomeSpan/HomeSpan/branches/all) 下的 README 文件详细说明了该版本可以使用哪个核心版本。

[^tested]: 更高版本可能可以正常运行，但尚未测试。注意：HomeSpan 不支持使用 Arduino-ESP32 核心的 alpha、beta 或预发布候选版本——测试仅在核心的正式版上进行。

[^homehub]: Apple 的 HomeKit 架构[需要使用 Home Hub](https://support.apple.com/en-us/HT207057)（HomePod 或 Apple TV）才能完整正常运行任何 HomeKit 设备，包括基于 HomeSpan 的设备。***不支持在没有 Home Hub 的情况下使用 HomeSpan。***

[^partition]: 此建议假设 ESP 设备具有 4MB 闪存。对于具有 8MB 或更多闪存的设备，您可以选择任何提供至少 1.9MB APP 空间和 1.9MB OTA 空间（如果使用 OTA）的分区方案。

### HomeSpan 亮点

- 提供自然、直观且**非常**易于使用的框架
- 利用独特*以服务为中心*的方法创建 HomeKit 设备
- 充分利用广受欢迎的 Arduino IDE
- 完全符合 HAP-R2 标准
- 数十种集成 HomeKit 服务
- 支持附件模式和桥接模式
- 支持通过与设置码或二维码配对
- 支持与家庭无线网络和以太网连接
- 支持 IPv4 和 IPv6 地址的双栈使用

### 针对 HomeSpan 开发人员

- Arduino 串行监视器的广泛使用
  - 实时、易于理解的诊断
  - 对每个底层 HomeKit 操作、数据请求和数据响应完全透明
  - 具有各种信息、调试和配置命令的命令行界面
- 内置数据库验证，确保配置满足所有 HAP 要求
- 利用 ESP32 的 16 通道 PWM 外设的专用类，可轻松控制：
  - LED 亮度（包括自动淡化）
  - 伺服电机
- 集成按钮和拨动开关功能，支持单击、双击和长按：
  - 将 ESP32 引脚连接到地或 VCC 的物理按钮
  - 连接到 ESP32 引脚的触摸板/传感器（用于支持触摸板的 ESP32 设备）
- 集成对 ESP32 片上远程控制外设的访问，可轻松生成红外和射频信号
- 控制单线和双线可寻址 RGB 和 RGBW LED 和 LED 灯带的专用类
- 专门用于控制步进电机的类，可以在后台平稳运行，而不会干扰 HomeSpan 设备
- 使用 ESP-NOW 在 ESP32 设备之间实现点对点通信的专用类
- 用于用户定义的日志消息的集成网络日志
- 带有大量注释的教程示例，将带你体验从 HomeSpan 的基础知识到高级 HomeKit 主题
- 展示 HomeSpan 实现的其他示例和项目
- 一套完整的文档，全面解释了 HomeSpan 的各种 API

### 针对 HomeSpan 终端用户

- 嵌入式无线网络接入点和网络界面，允许最终用户（非开发人员）：
  - 使用自己的家庭无线凭据设置 HomeSpan
  - 创建自己的 HomeKit 配对设置代码
- 状态指示灯和控制按钮，允许最终用户：
  - 强制解除设备与 HomeKit 的配对
  - 执行恢复出厂设置
  - 启动无线网络接入点
- 独立、详细的最终用户指南

## ❗最新更新 - HomeSpan 2.1.6（2025 年 10 月 26 日）

### 新功能

* **HomeSpan 现在支持 ESP32-C5！**

  * ESP32-C5 能够使用 **5.0 GHz** 和 **2.4 GHz** WiFi 频段
    * 在所有日志文件输出中添加 WiFi 频段信息，以指示正在使用哪个频段
  * 如要求 ESP-C5 使用 5.0 GHz 频段，请将以下内容添加到草图中的 `setup()` 函数中:
    * `WiFi.STA.begin(); WiFi.setBandMode(WIFI_BAND_MODE_5G_ONLY);`
    * 注意： `setBandMode()` 仅在 Arduino-ESP32 Core 3.3.0 或更高版本中可用
  * 请参阅 [WiFi 和以太网连接](docs/Networks.md)  以了解详细信息

### 更新和更正

* **更新了 OTA 密码存储，使用 SHA256 而不是 MD5 哈希，以符合最新的 ArduinoOTA 库协议**
  * 为了与之前版本的 Core 保持向后兼容，HomeSpan 仅在 Core 3.3.2 或更高版本下编译时才使用 SHA256 哈希，否则继续使用 MD5
  * 如果您使用 `homeSpan.enableOTA(char *pwd)` 在草图中设置 OTA 密码，则新的哈希将自动进行
  * 如果您之前使用 "O" 命令将密码输入到串行监视器中，则需要重新输入，以便 HomeSpan 可以将其保存为 SHA256
    * 如果没有重新输入，使用 Core 3.3.2 或更高版本上传 OTA 草图仍然有效，但诊断消息会警告您从 MD5 迁移到 SHA256 哈希
  * 为了便于在草图中将密码设置为**哈希值**而不是**纯文本** ， `homeSpan.enableOTA(char *pwd)` 已被修改，以便如果 *pwd* 以 "0x" 开头，后跟 64 个十六进制字符，则 *pwd* 将被解释为 SHA256 哈希值而不是纯文本，并且 HomeSpan 将直接存储它而不是先对其进行哈希处理
    * 在草图中指定 OTA 密码的哈希值比指定纯文本密码更安全
    * 对于无法轻松连接到串行端口的设备很有用，这会阻止您使用 "O" 命令输入 OTA 密码

* **添加了新的 "c" 命令，该命令将 HomeSpan 在初始启动期间显示的相同芯片和草图配置信息输出到串行监视器**
              
有关此版本中包含的所有更改和错误修复的详细信息，请参阅 [发布](https://github.com/HomeSpan/HomeSpan/releases) 更新。


[HomeSpan 更新明细](updatelist.md)

# HomeSpan 资源<a name="resources"></a>

HomeSpan 包括以下文档：

- [HomeSpan 入门](docs/GettingStarted.md) - 设置开发 HomeSpan 设备所需的软件和硬件
- [HomeSpan API 概述](docs/Overview.md) - HomeSpan API 概述，包括开发第一个 HomeSpan 示例的分步指南
- [HomeSpan 教程](docs/Tutorials.md) - HomeSpan 的示例指南
- [HomeSpan 服务和特性](docs/ServiceList.md) - HomeSpan 支持的所有 HAP 服务和特性的列表
- [HomeSpan 附件类别](docs/Categories.md) - HomeSpan 定义的所有 HAP 附件类别的列表
- [HomeSpan 命令行界面](docs/CLI.md) - 配置 HomeSpan 设备的无线凭据，修改其设置代码，监控和更新其状态，并从 Arduino IDE 串行监视器访问详细的实时设备诊断
- [HomeSpan 无线和以太网连接](docs/Networks.md) - HomeSpan 无线网络和以太网连接选项的高级讨论
- [HomeSpan 用户指南](docs/UserGuide.md) - 有关如何配置已编程的 HomeSpan 设备的无线凭据、修改其设置代码以及将设备与 HomeKit 配对的交钥匙式说明。不需要电脑！
- [HomeSpan API 参考](docs/Reference.md) - HomeSpan 库 API 的完整指南
- [HomeSpan 二维码](docs/QRCodes.md) - 创建并使用二维码来配对 HomeSpan 设备
- [HomeSpan OTA](docs/OTA.md) - 直接从 Arduino IDE 无线更新示例，无需串行连接
- [HomeSpan 看门狗定时器](docs/WDT.md) - 可选的保护功能，如果您的示例长时间挂起或冻结，可以触发自动重启
- [HomeSpan PWM](docs/PWM.md) - 使用 ESP32 的片载 PWM 外设集成控制标准 LED 和伺服电机
- [HomeSpan 射频控制](docs/RMT.md) - 使用 ESP32 的片载 RMT 外设，轻松生成射频和红外遥控信号
- [HomeSpan 灯带](docs/Pixels.md) - 可寻址的单线和双线 RGB 和 RGBW LED 和 LED 灯带的集成控制
- [HomeSpan 控制步进电机](docs/Stepper.md) - 步进电机集成控制，包括 PWM 微步进
- [HomeSpan Span 热点](docs/NOW.md) - 使用 ESP-NOW 促进 ESP32 设备之间的点对点双向通信
- [HomeSpan 电视服务](docs/TVServices.md) - 如何使用 HomeKit 未记录的电视服务和特性
- [HomeSpan 消息日志记录](docs/Logging.md) - 如何生成在 Arduino 串行监视器上显示的日志消息，以及可选地发布到集成的网络日志页面
- [HomeSpan TLV8 特性](docs/TLV8.md) - 用于创建 TLV8 对象以与基于 TLV8 的特性一起使用的类和方法
- [HomeSpan 设备复制](docs/Cloning.md) - 将损坏的设备无缝更换为新设备，而无需重新配对和丢失 HomeKit 自动化
- [HomeSpan 项目](https://github.com/topics/homespan) - HomePan 库的实际应用
- [HomeSpan 常见问题解答](docs/FAQ.md) - 常见问题解答
- [常见问题的解决方案](docs/Solutions.md) - 解决使用/编译 HomeSpan 时的一些常见问题
- [HomeSpan 推荐示例](https://github.com/abackup/HomeSpanReferenceSketches-Chinese) - 一系列独立的参考示例，展示了一些更复杂的 HomeKit 服务，如恒温器和灌溉系统

请注意，所有文档都是受版本控制的，并与每个分支相关联。*master* 分支通常指向最新版本。当可用时，该 *dev* 分支将包含正在开发的代码。

# 外部资源

除了 HomeSpan 资源之外，刚接触 HomeKit 编程的开发人员可能会发现 Apple 的 HomeKit 附件协议规范非商业版本 R2 ([HAP-R2](../master/docs/HAP-R2.pdf)) 的第 8 章和第 9 章非常有用。不幸的是， Apple 不再提供这份文件（也许是因为它最后一次更新是在 2019 年 7 月，现在已经有些过时了）。但是，你可以在网络上的其他地方找到此文档的副本。请注意，Apple 并未将 HAP-R2 文档替换为用于非商业版本的任何其他版本，并且 Apple 的开源 [HomeKit ADK](https://github.com/apple/HomeKitADK) 仅反映了原始的 HAP-R2 规格（而不是用于商业设备的 HomeKit 中提供的所有最新服务和特性）。

---

### Matter 和 Thread

由于 HomeSpan 完全围绕 HAP-R2 构建，因此没有计划使 HomeSpan 与 Matter 兼容。此外，Apple 和乐鑫都发布了供公众使用的 Matter SDK，因此无需再额外开发 Matter SDK。

### 反馈或问题？

请将 HomeSpan 的错误报告或其他问题提交至 [Issues](https://github.com/HomeSpan/HomeSpan/issues) 页面。请将有关 HomeSpan 的所有其他问题（使用、功能、规格、示例等）或你对新功能的任何想法、建议或有关 HomeSpan 或 HomeKit 的任何反馈发布到 [Discussion](https://github.com/HomeSpan/HomeSpan/discussions) 页面。对于与特定 HomeSpan 问题或与功能无关的更一般的问题或反馈，你可以直接 [homespan@icloud.com](mailto:homespan@icloud.com) 给我发电子邮件。

### 关于作者

HomeSpan 由 Gregg Berman 开发并持续维护和支持。它最初是为了解决无法用 Siri 操作射频控制的厨房通风罩的麻烦问题。我希望你会发现它很有用，而且使用起来很有趣。

这是我的第二个大型开源项目——我的第一个项目是设计一个开源系统，用于操作铁路模型，仅使用 Arduino Uno 和 Arduino Motor Shield 来生成数字命令和控制 (DCC) 信号。虽然我已经多年没有参与铁路模型爱好的建设，但展示我的原始系统（称为 DCC++）的视频，以及它如何工作的详细教程，仍然可以在 [DCC++ YouTube 频道](https://www.youtube.com/@dcc2840/videos) 上找到。
