<!--  原文时间：2025.2.9，校对时间：2025.2.27  -->

# WiFi 和以太网连接

HomeSpan 可以通过 WiFi 或以太网连接到您的家庭网络。 HomeSpan 支持的所有 ESP32 芯片都带有内置 WiFi，因此不需要额外的硬件。只有少数 ESP32 开发板配备以太网接口（例如 [Silicognition wESP32](https://wesp32.com) ），因此需要额外的硬件（例如 [Adafruit 以太网 FeatherWing](https://www.adafruit.com/product/3201) ）通过以太网进行连接。

## HomeSpan WiFi 连接

除非 HomeSpan 检测到您安装并配置了以太网接口（见下文），否则它将默认使用 WiFi 连接到您的家庭网络。要建立连接，HomeSpan 需要您网络的 SSID 和密码（您的 “WiFi 凭据”），它会将其保存在非易失性存储 (NVS) 中，以便在断电时也能保留。启动时，HomeSpan 会检查其 NVS 中是否有您的 WiFi 凭证。如果找到，它们将用于建立 WiFi 连接。如果没有，HomeSpan 会向串行接口输出一条消息（如果您的设备已连接到计算机），并使 HomeSpan 状态 LED 指示灯闪烁（如果您已添加），表明它需要您提供 WiFi 凭据。有以下三种方法：

1. **通过串行监视器输入您的 WiFi 凭证**

    * 如果您的设备通过 USB 连接到计算机，您可以在串行监视器命令行界面（“CLI”）中键入 “W”。 HomeSpan 将扫描您的 WiFi 环境并显示找到的所有网络 SSID。然后，您可以选择要使用的 SSID，或键入您自己的 SSID，并提供其密码。 HomeSpan 将此信息存储在 NVS 中，重新启动，然后使用这些 WiFi 凭据连接到您的网络。
1. **通过 HomeSpan 设置接入点输入您的 WiFi 凭证**

    * 如果您的设备未连接到计算机，但您已添加 HomeSpan 控制按钮和 HomeSpan 状态 LED，则可以激活 HomeSpan 命令模式（有关详细信息，请参阅 [用户指南](UserGuide.md)）并启动 HomeSpan 设置接入点。这将在您的设备上启动一个强制 WiFi 接入点，该接入点托管 HomeSpan 设置 Web 表单，您可以在其中输入家庭网络的 WiFi 凭据。进入 HomeSpan 后，将使用这些凭据连接到您的网络，如果成功，则将凭据保存在 NVS 中，重新启动，然后按照上述方式继续。需要注意的一些事项：

      * 您也可以通过键入 “A” 从 CLI 启动 HomeSpan 设置接入点。但是，这主要用于测试目的，因为如果您的设备已通过 USB 连接到计算机，您不妨使用上面的 “W” 方法
      * 您可以使用以下 homeSpan 方法更改 HomeSpan 设置接入点的名称、密码和超时： `setApSSID()` 、 `setApPassword()`和`setApTimeout()` （有关所有 homeSpan 方法的详细信息，请参阅 [API 参考](Reference.md) 页面）
      * 如果您将 homeSpan `enableAutoStartAP()`方法添加到您的草图中，HomeSpan 将在启动时找不到存储在 NVS 中的任何 WiFi 凭证时_自动_启动其设置接入点
      * 如果您确实不喜欢 HomeSpan 设置接入点的外观和感觉，您可以通过在草图中实现 homeSpan `setApFunction()`方法来创建自己的方法，让 HomeSpan 获取您的 WiFi 凭证
      * 是的，Apple 确实有一种更强大的方法，可以让基于 WiFi 的 HomeKit 设备无缝地从您的 iPhone 获取 WiFi 凭证，但是，HomeSpan 无法使用此方法，因为 Apple 将此方法限制为仅适用于获得许可的商业 HomeKit 设备
      
1. **在 HomeSpan 草图中对 WiFi 凭证进行硬编码**

    * 如果上述方法都没有吸引力，您可以选择使用 homeSpan `setWifiCredentials()` 方法将您的 WiFi 凭证直接硬编码到您的草图中，尽管出于安全原因这不被认为是一个好的做法。

当 HomeSpan 尝试使用您的 WiFi 凭证连接到您的家庭网络时，它会发出请求，然后等待响应。如果在一段时间后没有连接，它会发出另一个请求并等待更长的时间以获得响应。每次尝试失败后，HomeSpan 都会使用指定的模式增加等待时间，然后重复该模式。默认模式是先等待 5 秒，然后等待 8、14、22、36 和 60 秒，之后该模式会在 5 秒后重新开始。如果需要，您可以使用 homeSpan `setConnectionTimes()` 方法创建自己的请求/响应等待时间模式。

连接后，如果连接丢失，HomeSpan 会自动管理所有重新连接（使用与上面相同的请求/响应等待模式）。如果您启用了 Web 日志记录，则会记录所有断开连接和重新连接。

如果您的家庭网络基于网状路由器，并且多个接入点共享相同的 SSID，HomeSpan 会自动连接具有最强 RSSI 信号的接入点。作为一个选项，您可以将 homeSpan `enableWiFiRescan()` 方法添加到您的草图中，让 HomeSpan 定期重新扫描您的 WiFi 网络，以查看是否存在具有相同 SSID 的更强接入点。如果找到一个接入点，HomeSpan 会断开与现有接入点的连接，并重新连接到更强的接入点。在串行监视器和 Web 日志（如果启用）中，HomeSpan 指示其所连接的特定接入点的 BSSID（即 6 字节 MAC 地址）。作为一个选项，您可以使用 homeSpan `addBssidName()` 方法将 BSSID 映射到自定义显示名称。指定后，HomeSpan 将在日志文件中的任何 BSSID 旁边显示这些显示名称，以便更轻松地跟踪正在使用的接入点。

在内部，HomeSpan 使用 Arduino-ESP32 库的全局 `WiFi` 对象管理 WiFi 连接，并使用 `WiFi.begin()` 方法启动连接。此方法假设连接到需要 SSID 和密码的标准 WiFi 网络。如果您尝试连接到企业 WiFi 网络，或者在连接到 WiFi 网络时需要进行其他特殊配置要求（例如更改 WiFi 天线的功率），您可以创建自己的 "begin" HomeSpan 的函数将通过在草图中实现 homeSpan `setWifiBegin()` 方法来调用。

最后，如果您需要一次或每次建立 WiFi 连接（或断开连接后重新建立）时调用任何其他函数，您可以在草图中实现 homeSpan `setConnectionCallback()` 方法。

## HomeSpan 以太网连接

HomeSpan 利用 Arduino-ESP32 库的全局 `ETH` 对象来管理以太网连接，因此支持 Arduino-ESP32 库支持的任何以太网板。要建立以太网连接，只需在调用 `homeSpan.begin()` 之前在草图的 `setup()` 部分中添加 `ETH.begin()` 即可。 `ETH.begin()` 函数需要不同的参数，具体取决于您使用的特定以太网接口或外部板。请参阅以太网接口/板附带的说明，并使用 Arduino-ESP32 库中乐鑫提供的以太网示例来确定板的正确参数。

您无需在 HomeSpan 草图中添加任何内容来通知它您将使用以太网连接而不是 WiFi。相反，在启动时，如果 HomeSpan 检测到以太网接口设备已使用 `ETH.begin()` 正确_配置和初始化，HomeSpan 将自动切换到 “以太网模式”，并使用以太网而不是 WiFi 来进行与家庭网络之间的所有通信。请注意，以太网电缆本身不需要插入路由器即可让 HomeSpan 在启动过程中切换到“以太网模式”。

但是，如果由于某种原因 HomeSpan 无法自动检测已配置的以太网接口，或者使用 `ETH.begin()` 对以太网接口的初始化是在主 Arduino `setup()` 函数之外完成的，则可以通过在 `homeSpan.begin()` 之前调用 `homeSpan.useEthernet()` 来强制 HomeSpan 使用以太网模式而不是 WiFi。

与 WiFi 连接类似，HomeSpan 会自动处理所有以太网断开/重新连接（例如，如果您拔下以太网电缆，然后将其插回路由器，或者路由器本身重新启动），并将此类事件记录在 Web 日志中（如果启用）。与使用 WiFi 类似，要运行自定义函数一次或每次建立以太网连接（或断开连接后重新建立），您可以在草图中实现 homeSpan `setConnectionCallback()` 方法。

IPv6 Compatability

The Arduino-ESP32 library and the Espressif IDF natively supports the simultaneous use of both IPv4 and IPv6 addresses, though unless IPv6 addresses are enabled, the default behavior for any given sketch is to use only IPv4.[^ipv6] To enable IPv6 addresses on the ESP32 WiFi interface, add `WiFi.enableIPv6()` to your sketch. To enable IPv6 addresses on the ESP32 Ethernet interface, add `ETH.enableIPv6()` to your sketch.

[^ipv6]:IPv4 addresses are 4 bytes long and usually written as 4 decimal numbers (from 0-255) separated by periods, such as 192.168.1.10. IPv6 addresses are 16 bytes long and usually written as 8 groups of 2 bytes each, separated by colons, such as 2001:0db8:85a3:0000:0000:8a2e:0370:7334. By convention, leading zeros for each 2-byte group are usually omitted, and the longest *repeated* group of zero can be abbreviated by "::" as such: 2001:db8:85a3::8a2e:370:7334.

When the use of IPv6 addresses is enabled, HomeSpan will automatically handle all HTTP requests received from either an IPv4 or IPv6 address. Note that whereas the ESP32 will typically receive only one IPv4 adddress from the router when connecting to a network, it may receive up to three IPv6 addresses: a *Link Local Address*, a *Unique Local Address*, and (optionally) a *Global Address*. The exact order in which IPv4 and IPv6 addresses are acquired is indeterminant and can change everytime the device reboots and tries to connect to your network.

HomeSpan considers network connectivity to be established upon receiving the first IP address (whether IPv4 or IPv6) and will call any user-defined callback function set by `homeSpan.setConnectionCallback()` *only once upon reception of the first address*. HomeSpan does not call the callback when receiving any additional IP addresses but it does creates a Web Log entry (along with a report to the Serial Monitor) for each IP address when acquired.[^events]

[^events]:If you need more advanced callback handling for network connectivity events, such as calling a function for *each* IP address acquired (instead of just the first one), please use the Arduino-ESP32's built-in network event handling methods: `WiFi.onEvent()` and `ETH.onEvent()`, or more generically, `Network.onEvent()`.

In addition, HomeSpan reports the device's IPv6 *Unique Local Address* alongside its IPv4 address whenever displayed in either the Serial Monitor or the Web Log. If IPv6 addressing has not been enabled, or if it has been enabled but an IPv6 *Unique Local Address* has not been acquired, the IPv6 address will be shown as "::".


---

[↩️](../README.md#resources)返回欢迎页面
