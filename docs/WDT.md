<!-- 原文时间：2025.2.9，校对时间：校对时间：2025.4.9 -->

# HomeSpan 看门狗定时器

ESP32 操作系统支持基于任务的看门狗定时器 (TWDT)，该定时器会检查以确保订阅任务看门狗的任务（或任何选定的代码段）正在定期运行。如果没有，则在预先指定的时间段后，设备会自动重新启动。

通常，只有后台 IDLE 进程订阅 TWDT，<mark>但默认情况下，Arduino-ESP32 库不会为单核芯片（ESP32-S2、C3 和 C6）启用 IDLE 任务看门狗，并且仅为双核芯片（ESP32 和 ESP32-S3）启用与 Core-0 关联的 IDLE 任务看门狗</mark>。Arduino-ESP32 库默认将 TWDT 超时设置为 5 秒。

基于这些默认值，如果您的示例挂起或以其他方式进入无限循环，则设备将无限期冻结，直到您手动重置或关闭电源。

幸运的是，您可以向 TWDT 添加一个可选的看门狗，专门检查以确保所有需要的 HomeSpan 任务和功能都按预期运行。为此，只需在示例的 `setup()` 部分顶部附近调用 homeSpan 方法 `enableWatchdog(uint16_t nSeconds)`。启用后，TWDT 将超时并在 *nSeconds* 后自动重新启动设备，除非 HomeSpan 看门狗已重置。HomeSpan 在到达 HomeSpan `poll()` 函数的最后时会自动重置其看门狗。只要每 *nSeconds* 秒到达 `poll()` 函数的末尾，TWDT 就会保持沉默。但如果 `poll()` 函数挂起，或者另一个进程阻止调用 `poll()` 函数，则 TWDT 将在 *nSeconds* 秒后触发，设备将重新启动。

通过将 *nSeconds* 设置为足够长的时间以允许所有设置过程运行（可能为 10-20 秒），您可以确保如果任何 HomeSpan 过程陷入无限循环，设备将自行重启。这在与 OTA 回滚结合使用时特别有用，因为您可以配置一些东西，这样如果您通过 OTA 上传新示例并且它在启动时挂起，它将在短时间后重启并回滚到示例的先前工作版本。有关如何启用回滚的详细信息，请参阅 [OTA](OTA.md) 页面。

如果出于某种原因您需要在 HomeSpan 框架内执行一个函数，该函数将阻止 `poll()` 任务运行较长时间（即长于 *nSeconds*），您可以调用 `homeSpan.disableWatchdog()` 暂时禁用 HomeSpan 看门狗并防止重启。然后，当函数完成时，只需再次调用 `homeSpan.enableWatchdog(nSeconds)` 即可重新启用 HomeSpan 看门狗。有关 `enableWatchdog()` 和 `disableWatchdog()` 方法的详细信息，请参阅 [API 参考](Reference.md)。

### 实施说明

* 在内部，HomeSpan 调用 homeSpan `resetWatchdog()` 方法在 `poll()` 函数结束时重置其看门狗。HomeSpan 还在任何其他 HomeSpan 函数的末尾嵌入对 `resetWatchdog()` 的调用，这些函数在设计上会暂时阻止 `poll()` 函数，例如当 HomeSpan 等待您在串行监视器中输入无线凭据时、当您使用控制按钮将 HomeSpan 置于命令模式时，或者当您启动 HomeSpan 设置接入点时。这些额外的嵌入式 `resetWatchdog()` 调用确保 TWDT 不会在这些其他函数运行时触发，但如果这些函数本身挂起，TWDT 确实会触发并重新启动设备。

* `resetWatchdog()` 方法有两个用途：它重置 HomeSpan 看门狗（如果已启用）并调用 `vTaskDelay(1)`（无论 HomeSpan 看门狗是否已启用）。对 `vTaskDelay` 的调用允许 HomeSpan 让步 1 毫秒，以便其他具有相同或较低优先级的单独任务（例如 IDLE 任务）有机会运行。

* homeSpan `resetWatchdog()` 方法是公开的，如果需要，您可以在示例中自行调用它。例如，如果您的示例通过从主 Arduino `loop()` 中调用 `homeSpan.poll()` 来运行 HomeSpan `poll()` 函数，并且您在 `loop()` 中添加了运行时间超过 *nSeconds* 秒的代码，则您应该在运行该代码之前调用 `homeSpan.disableWatchdog()`，或者在长时间运行的代码中反复调用 `resetWatchdog()`，该调用仅在代码正常运行时才会到达，但如果代码挂起，则不会调用。这有效地允许 TWDT 监视您的长时间运行的代码并在挂起时触发重新启动。

* 如果您的示例不是从 Arduino `loop()` 中调用 `homeSpan.poll()`，而是在 Arduino `setup()` 函数末尾调用 `homeSpan.autoPoll()`，以在单独运行的任务中启动 HomeSpan 轮询，那么 HomeSpan 轮询任务将不受您可能添加到 Arduino `loop()` 的任何代码延迟的影响。

* 但是，即使您使用 `homeSpan.autoPoll()` 在其自己的任务中运行 HomeSpan 轮询，您仍需要确保不要向 Arduino `setup()` 函数添加任何长时间运行的代码，因为这些代码会将轮询的初始启动延迟超过 *nSeconds* 秒（从您调用 `homeSpan.enableWatchdog(nSeconds)` 开始）。如果您确实向 Arduino `setup()` 函数添加了任何需要很长时间运行的初始化代码，您可以等到该代码完成后再调用 `enableWatchdog(nSeconds)`，或者更好的是，您仍然可以在 Arduino `setup()` 函数的顶部调用 `enableWatchdog(nSeconds)`，然后从长时间运行的代码中定期调用 `resetWatchdog()`。这可确保如果您的代码挂起，HomeSpan 看门狗将在 *nSeconds* 秒后触发重新启动。

* 请注意，为了避免触发 HomeSpan 看门狗，HomeSpan `poll()` 函数需要每 *nSeconds* 秒到达其**结束**。这意味着您需要确保 `poll()` 函数不会在运行您用于实现 HomeSpan 服务结构的任何代码（例如，SpanService 构造函数、`update()`、`button()` 和 `loop()` 方法等）或您用于实现 HomeSpan 回调的任何代码时延迟运行。与上述类似，如果您确实需要向需要很长时间才能运行的服务结构或回调添加代码，请确保在该代码中定期调用 `homeSpan.resetWatchdog()`，以便它受到 HomeSpan 看门狗的适当保护。

* 启用 HomeSpan 看门狗只能通过在 *nSeconds* 秒后触发重启（如果看门狗未重置）来保护*您的示例*免于无限期挂起。但是，如果您的示例包含不会触发看门狗的长时间延迟，HomeSpan 看门狗不会创建任何类型的“心跳”来保持与 HomeKit 或您的“家庭”应用的连接。这意味着即使启用了 HomeSpan 看门狗保护，阻止 HomeSpan `poll()` 函数及时运行的延迟也会导致 HomeKit 在您的“家庭”应用中显示可怕的“无响应”消息。

* HomeSpan 看门狗是作为 ESP32 的*任务*看门狗定时器的一部分实现的，它与各种操作系统功能使用的 ESP32 的*中断*看门狗定时器 (IWDT) 完全分开。

---

[↩️](../README.md#resources) 返回欢迎页面
