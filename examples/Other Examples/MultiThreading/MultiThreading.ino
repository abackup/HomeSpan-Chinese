/*********************************************************************************
* MIT 许可证
*
* 版权所有 (c) 2020-2025 Gregg E. Berman
*
* https://github.com/HomeSpan/HomeSpan
*
* 特此授予任何人免费获得本软件及其相关文档文件（以下简称“软件”）副本的许可，
* 允许其不受限制地处理
* 本软件，包括但不限于以下权利：
* 使用、复制、修改、合并、发布、分发、再许可和/或出售
* 本软件的副本，并允许向其提供本软件的人员
* 这样做，但须遵守以下条件：
*
* 上述版权声明和本许可声明应包含在所有
* 本软件的副本或实质性部分中。
* 本软件按“原样”提供，不提供任何形式的明示或暗示的保证，
* 包括但不限于适销性保证、
* 特定用途适用性保证和不侵权保证。在任何情况下，
* 作者或版权所有者均不对任何索赔、损害或其他责任承担责任，
* 无论该责任是基于合同、侵权或其他原因，
* 也无论该责任是因本软件或其使用或与本软件相关的其他交易而产生。
* *
********************************************************************************/

// 此示例是示例 1（简易灯泡配件）的变体，演示了如何使用多线程功能，在主 HomeSpan 轮询进程之外的线程中周期性地切换灯泡的电源状态。
// 您可以像往常一样通过“家庭”App 开关灯泡配件，但灯泡每 10 秒会自动切换状态（如果当前关闭则打开，如果当前开启则关闭）。

// 请注意，此示例并未实现实际的 LED，仅展示了“家庭”App 中的工作原理。

// 此示例可用于单处理器和双处理器设备。

#include "HomeSpan.h"

Characteristic::On *power;                  // 新增！创建一个指向 On 特性的全局指针（将在下文中使用）

//////////////////////////////////////

void setup() {
 
  Serial.begin(115200);

  homeSpan.begin(Category::Lighting,"HomeSpan LightBulb");

  new SpanAccessory();
    new Service::AccessoryInformation();
      new Characteristic::Identify();
            
    new Service::LightBulb();
      power = new Characteristic::On();     // 新增！将指向特征值的指针保存到全局变量 power 中。

  homeSpan.autoPoll();                      // 新功能！启用自动轮询。HomeSpan 将在单独的线程中运行其轮询进程。
}

//////////////////////////////////////

void loop(){

  // 注意：我们没有在这个 loop() 中调用 homeSpan.poll()，因为我们已经在上面的单独线程中通过调用 homeSpan.autoPoll() 开始了轮询。

  delay(10000);         // 休眠 10 秒 - 注意，由于轮询过程在不同的线程中，因此这不会影响 HomeSpan。

  Serial.printf("*** Flipping power of Light!\n");

  homeSpanPAUSE;                        // 暂时暂停 HomeSpan 轮询过程
  power->setVal(1-power->getVal());     // 使用我们上面保存的指针翻转 On 特性的值
  
} // 注意：在 loop() 代码块结束时，HomeSpan 轮询会自动恢复（无需调用 homeSpanRESUME）。
  
