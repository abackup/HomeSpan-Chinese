HomeSpan：ESP32 的 HomeKit 实现

示例 1：由基本 HomeSpan 组件构建的无功能开/关灯控制 

#include "HomeSpan.h"        

void setup() {                
  Serial.begin(115200);       
  homeSpan.begin(Category::Lighting,"HomeSpan LightBulb");   
  new SpanAccessory();                              
    new Service::AccessoryInformation();            
      new Characteristic::Identify();                                                                   
  //  new Characteristic::Manufacturer("HomeSpan");  // 配件制造商（任意文本字符串，每个配件可以相同）
  //  new Characteristic::SerialNumber("123-ABC");   // 配件序列号（任意文本字符串，每个配件可以相同）
  //  new Characteristic::Model("120-Volt Lamp");    // 配件型号（任意文本字符串，每个配件可以相同）
  //  new Characteristic::FirmwareRevision("0.9");   // 配件固件（任意文本字符串，每个配件可以相同）
    new Service::LightBulb();                        
      new Characteristic::On();                      
} 
void loop(){
  homeSpan.poll();  
} 
