/*********************************************************************************
 *  MIT 许可证
 *  
 *  Copyright (c) 2020-2022 Gregg E. Berman
 *  
 *  https://github.com/HomeSpan/HomeSpan
 *  
 *  特此授予获得此软件和相关文档文件（“软件”）副本的任何人免费许可，以无限制方式处理软件，
 *  包括但不限于使用、复制、修改、合并、发布、分发、再许可和/或销售软件副本的权利，并允许
 *  向其提供软件的人员这样做，但须遵守以下条件：
 *  
 *  上述版权声明和本许可声明均应包含在软件的所有副本或重要部分中。
 *  
 *  软件按“原样”提供，不作任何明示或暗示的保证，包括但不限于适销性、特定用途的适用性和不
 *  侵权性的保证。在任何情况下，作者或版权持有者均不对因软件或使用或其他处理软件而引起的
 *  或与之相关的任何索赔、损害或其他责任承担责任，无论是合同行为、侵权行为还是其他行为。
 *  
 ********************************************************************************/
 
////////////////////////////////////////////////////////////
//                                                        //
//              HomeSpan：ESP32 的 HomeKit 实现           //
//    ------------------------------------------------    //
//                                                        //
//             示例 8：HomeKit 桥接器和桥接器配件          //
//                                                        //
////////////////////////////////////////////////////////////

#include "HomeSpan.h" 
#include "DEV_LED.h"     

void setup() {

  // 如果多附件设备的第一个附件中定义的唯一服务是必需的附件信息服务，则称该设备配置为“桥接器”。从历史上看，桥接设备和非桥接设备之间可能存在许多功能差异，
  // 但自 iOS 15 以来，功能差异并不明显，但有两个例外：

  // 1. 回想一下示例 7，使用 Characteristic::Name() 更改附件图块的默认名称对定义的第一个附件不起作用。“家庭”应用始终将第一个附件图块的默认名称显示为 homeSpan.begin() 
  // 中指定的设备名称。但是，在将设备实现为桥接器时，这不是问题，因为第一个附件就是桥接器本身——在这种情况下，让默认名称与设备名称匹配更有意义。更重要的是，您现在可以
  // 使用 Characteristic::Name() 更改两个 LED 附件图块的默认名称。

  // 2. 配置为桥接器的设备出现在 “家庭”应用的主设置页面下，该页面显示所有集线器和桥接器。

  // 下面的草图在功能上与示例 7 相同，不同之处在于我们定义三个附件（一个用于简单开/关 LED，一个用于可调光 LED），
  // 而不是定义两个附件，其中第一个附件充当桥接器。

  // 与往常一样，所有先前的评论都已删除，仅显示上一个示例中的新更改。

  // 注意：要了解其实际工作原理，您需要在加载新代码后取消配对您的设备并重新配对。

  Serial.begin(115200);

  // 下面我们将 Category::Lighting 替换为 Category::Bridges。这会更改与 “家庭”应用配对时显示的设备图标，
  // 但不会更改附件图块的图标。您可以选择任何您喜欢的类别。

  // 例如，即使我们将设备配置为 Bridge，我们也可以继续使用 Category::Lighting。

  homeSpan.begin(Category::Bridges,"HomeSpan Bridge");
  
  new SpanAccessory();                            // 第一个附件是新的“桥接”附件。它不包含任何功能服务，只包含附件信息服务
    new Service::AccessoryInformation();
      new Characteristic::Identify();            
 
  new SpanAccessory();                            // 第二个附件与示例 7 中的第一个附件相同，不同之处在于 Characteristic::Name() 现在执行一些操作
    new Service::AccessoryInformation();
      new Characteristic::Identify();            
      new Characteristic::Name("Simple LED");     // 请注意，与示例 7 不同，Name() 的这种用法现在由 “家庭”应用使用，因为它不是第一个配件（上面的 Bridge 是第一个）
    new DEV_LED(16);

  new SpanAccessory();                            // 此第三个附件与示例 7 中的第二个附件相同
    new Service::AccessoryInformation();    
      new Characteristic::Identify();               
      new Characteristic::Name("Dimmable LED");  
    new DEV_DimmableLED(17);

} // setup() 结束

//////////////////////////////////////

void loop(){
  
  homeSpan.poll();
  
} // loop() 结束
