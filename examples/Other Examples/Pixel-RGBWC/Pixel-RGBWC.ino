/*********************************************************************************
 *  MIT License
 *  
 *  Copyright (c) 2024 Gregg E. Berman
 *  
 *  https://github.com/HomeSpan/HomeSpan
 *  
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *  
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *  
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *  
 ********************************************************************************/
 
// HomeSpan 可寻址 RGBCW LED 示例。演示如何使用两个独立的配件控制单个 NeoPixel LED 灯带。第一个配件控制 RGB LED，第二个配件控制暖白光和冷白光 LED。
// 两组配件可以同时开启。
//
// 请注意，使用 Pixel::CCT() 方法将色温转换为白光 LED 的色温值。
//
// 另请注意，HomeKit 使用 Mirek 色温标度而非开尔文色温标度来表示色温。要将两者进行转换，请将两者除以 100 万：Mirek=1.0e6/开尔文，开尔文=1.0e6/Mirek。
//

#ifndef HS_FEATHER_PINS
  #define NEOPIXEL_PIN    23        // 更改此引脚以匹配您的主板
#else
  #define NEOPIXEL_PIN    F23       // FEATHER PIN MAPPING - FOR HOMESPAN DEVELOPER USE ONLY
#endif
 
#define NPIXELS          100    // 灯串中可控像素的数量（可能小于实际 LED 数量）
#define WARM_LED_TEMP    3000   // 暖白光 LED 的温度（单位：开尔文）
#define COOL_LED_TEMP    6500   // 冷白光 LED 的温度（单位：开尔文）
 
#include "HomeSpan.h"

Pixel pixel(NEOPIXEL_PIN,"GRBWC");     // 创建一个支持 RGB LED 以及暖白光和冷白光 LED 的像素链全局实例
Pixel::Color colorRGB;                 // 创建一个像素颜色的全局实例，用于存储 RGB 颜色
Pixel::Color colorWC;                  // 创建一个像素颜色的全局实例，用于存储 WC 颜色

///////////////////////////////

struct NeoPixel_RGB : Service::LightBulb {      // 定义一个 RGB 灯泡来控制 NeoPixel 灯带上的 RGB LED
 
  Characteristic::On power{0,true};
  Characteristic::Hue H{0,true};
  Characteristic::Saturation S{0,true};
  Characteristic::Brightness V{100,true};
  
  NeoPixel_RGB() : Service::LightBulb(){
    
    V.setRange(5,100,1);
    update();                                 // 手动调用 update() 来设置像素并恢复其初始值
  }

  boolean update() override {

    colorRGB=pixel.HSV(H.getNewVal(), S.getNewVal(), V.getNewVal()*power.getNewVal());
    pixel.set(colorRGB+colorWC,NPIXELS);          
    return(true);  
  }
};

///////////////////////////////

struct NeoPixel_WC : Service::LightBulb {      // 定义 WC 灯泡来控制 NeoPixel 灯带上的暖白和冷白 LED
 
  Characteristic::On power{0,true};
  Characteristic::Brightness V{100,true};
  Characteristic::ColorTemperature T{(uint32_t)1.0e6/((COOL_LED_TEMP+WARM_LED_TEMP)/2),true};     // set initial value to average of Warm and Cool Temp
  
  NeoPixel_WC() : Service::LightBulb(){

    V.setRange(5,100,1);
    T.setRange(1.0e6/COOL_LED_TEMP,1.0e6/WARM_LED_TEMP);      // set range of control to match range of Warm-White and Cool-White LEDs
    
    update();                                 // manually call update() to set pixel with restored initial values
  }

  boolean update() override {

    colorWC=pixel.CCT(1.0e6/T.getNewVal(), V.getNewVal()*power.getNewVal());   // convert HomeKit temperature (in Mirek) to Kelvin
    pixel.set(colorRGB+colorWC,NPIXELS);          
    return(true);  
  }
};

///////////////////////////////

void setup() {
  
  Serial.begin(115200);

  pixel.setTemperatures(WARM_LED_TEMP,COOL_LED_TEMP);        // set color temperatures of Warm-White and Cool-White LEDs
 
  homeSpan.begin(Category::Lighting,"RGBWC Light");

  SPAN_ACCESSORY();

  SPAN_ACCESSORY("Neo RGB");
    new NeoPixel_RGB();

  SPAN_ACCESSORY("Neo WC");
    new NeoPixel_WC();
}

///////////////////////////////

void loop() {
  homeSpan.poll();
}

///////////////////////////////
