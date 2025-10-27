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

#### 与 WiFi Mesh 网络一起使用

如果您的家庭网络基于网状路由器，并且多个接入点共享相同的 SSID，HomeSpan 会自动连接具有最强 RSSI 信号的接入点。作为一个选项，您可以将 homeSpan `enableWiFiRescan()` 方法添加到您的草图中，让 HomeSpan 定期重新扫描您的 WiFi 网络，以查看是否存在具有相同 SSID 的更强接入点。如果找到一个接入点，HomeSpan 会断开与现有接入点的连接，并重新连接到更强的接入点。在串行监视器和 Web 日志（如果启用）中，HomeSpan 指示其所连接的特定接入点的 BSSID（即 6 字节 MAC 地址）。作为一个选项，您可以使用 homeSpan `addBssidName()` 方法将 BSSID 映射到自定义显示名称。指定后，HomeSpan 将在日志文件中的任何 BSSID 旁边显示这些显示名称，以便更轻松地跟踪正在使用的接入点。

#### 与企业 WiFi 网络或专用 WiFi 配置一起使用

在内部，HomeSpan 使用 Arduino-ESP32 库的全局 `WiFi` 对象管理 WiFi 连接，并使用 `WiFi.begin()` 方法启动连接。此方法假设连接到需要 SSID 和密码的标准 WiFi 网络。如果您尝试连接到企业 WiFi 网络，或者在连接到 WiFi 网络时需要进行其他特殊配置要求（例如更改 WiFi 天线的功率），您可以创建自己的 "begin" HomeSpan 的函数将通过在草图中实现 homeSpan `setWifiBegin()` 方法来调用。

#### 连接回调

如果您需要一次或每次建立 WiFi 连接（或断开连接后重新建立）时调用任何其他函数，您可以在草图中实现 homeSpan `setConnectionCallback()` 方法。

#### WiFi 频段：2.4 GHz 对比 5.0 GHz

HomeSpan 支持的所有 ESP32 芯片均包含 2.4 GHz WiFi 频段，这是大多数优先考虑覆盖范围而非速度的物联网设备的默认配置。然而，ESP32-C5 芯片还包含 5.0 GHz WiFi 频段。要在 ESP32-C5 上启用 5.0 GHz 频段，请在代码的 setup() 函数中添加以下内容：

```
WiFi.STA.begin();
WiFi.setBandMode(WIFI_BAND_MODE_5G_ONLY);
```
请注意，`setBandMode()` 仅受 Arduino-ESP32 Core 3.3.0 或更高版本支持。

## HomeSpan 以太网连接

HomeSpan 利用 Arduino-ESP32 库的全局 `ETH` 对象来管理以太网连接，因此支持 Arduino-ESP32 库支持的任何以太网板。要建立以太网连接，只需在调用 `homeSpan.begin()` 之前在草图的 `setup()` 部分中添加 `ETH.begin()` 即可。 `ETH.begin()` 函数需要不同的参数，具体取决于您使用的特定以太网接口或外部板。请参阅以太网接口/板附带的说明，并使用 Arduino-ESP32 库中乐鑫提供的以太网示例来确定板的正确参数。

您无需在 HomeSpan 草图中添加任何内容来通知它您将使用以太网连接而不是 WiFi。相反，在启动时，如果 HomeSpan 检测到以太网接口设备已使用 `ETH.begin()` 正确_配置和初始化，HomeSpan 将自动切换到 “以太网模式”，并使用以太网而不是 WiFi 来进行与家庭网络之间的所有通信。请注意，以太网电缆本身不需要插入路由器即可让 HomeSpan 在启动过程中切换到“以太网模式”。

但是，如果由于某种原因 HomeSpan 无法自动检测已配置的以太网接口，或者使用 `ETH.begin()` 对以太网接口的初始化是在主 Arduino `setup()` 函数之外完成的，则可以通过在 `homeSpan.begin()` 之前调用 `homeSpan.useEthernet()` 来强制 HomeSpan 使用以太网模式而不是 WiFi。

#### 连接回调

与 WiFi 连接类似，HomeSpan 会自动处理所有以太网断开/重新连接（例如，如果您拔下以太网电缆，然后将其插回路由器，或者路由器本身重新启动），并将此类事件记录在 Web 日志中（如果启用）。与使用 WiFi 类似，要运行自定义函数一次或每次建立以太网连接（或断开连接后重新建立），您可以在草图中实现 homeSpan `setConnectionCallback()` 方法。

## IPv6 兼容性

Arduino-ESP32 库和 Espressif IDF 原生支持同时使用 IPv4 和 IPv6 地址，但除非启用 IPv6 地址，否则任何给定草图的默认行为都是仅使用 IPv4。[^ipv6] 要在 ESP32 WiFi 接口上启用 IPv6 地址，请在草图中添加 `WiFi.enableIPv6()`。要在 ESP32 以太网接口上启用 IPv6 地址，请在草图中添加 `ETH.enableIPv6()`。

[^ipv6]:IPv4 地址长度为 4 个字节，通常写为 4 个十进制数字（0-255），以句点分隔，例如 192.168.1.10。 IPv6 地址长度为 16 个字节，通常写成 8 组，每组 2 个字节，以冒号分隔，例如 2001:0db8:85a3:0000:0000:8a2e:0370:7334。按照惯例，每个 2 字节组的前导零通常会被省略，最长的 *重复* 零组可以用“::”缩写，例如：2001:db8:85a3::8a2e:370:7334。

启用 IPv6 地址后，HomeSpan 将自动处理从 IPv4 或 IPv6 地址接收的所有 HTTP 请求。请注意，ESP32 在连接到网络时通常只会从路由器接收一个 IPv4 地址，但它最多可以接收三个 IPv6 地址：一个 *链路本地地址*、一个 *唯一本地地址* 和一个（可选） *全局地址*。获取 IPv4 和 IPv6 地址的具体顺序是不确定的，并且每次设备重启并尝试连接到网络时都可能发生变化。

HomeSpan 认为在收到第一个 IP 地址（无论是 IPv4 还是 IPv6）时网络连接已建立，并且只会在收到第一个地址时调用一次由 `homeSpan.setConnectionCallback()` 设置的用户自定义回调函数。HomeSpan 在收到任何其他 IP 地址时不会调用回调函数，但它会在获取每个 IP 地址时创建一个 Web 日志条目（以及向串行监视器发送的报告）。[^events]

[^events]:如果您需要更高级的网络连接事件回调处理，例如为获取的每个 IP 地址（而不仅仅是第一个）调用一个函数，请使用 Arduino-ESP32 的内置网络事件处理方法：`WiFi.onEvent()` 和 `ETH.onEvent()`，或者更通用的 `Network.onEvent()`。

此外，无论设备在串口监视器还是 Web 日志中显示，HomeSpan 都会报告其 IPv6 *唯一本地地址* 及其 IPv4 地址。如果未启用 IPv6 寻址，或者已启用但未获取 IPv6 *唯一本地地址*，则 IPv6 地址将显示为“::”。


---

[↩️](../README.md#resources)返回欢迎页面
