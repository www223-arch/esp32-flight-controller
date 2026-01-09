#include <WiFi.h>
#include "ps2.h"
// 创建PS2控制器实例（可使用默认引脚或自定义引脚）
PS2Controller ps2;  // 默认引脚
//PS2Controller ps2(GPIO_NUM_21, GPIO_NUM_19, GPIO_NUM_18, GPIO_NUM_5); // 自定义引脚
void setup() {
  Serial.begin(115200);
      ps2.init(); 
}
void loop() {
        // 获取按下的按键
        uint8_t key = ps2.getPressedKey();
        if (key != 0) {
            printf("Pressed Key: %d\n", key);
        }
        
        // 获取左右摇杆值
        uint8_t lx = ps2.getAnalogValue(PS2AnalogStick::LX);
        uint8_t ly = ps2.getAnalogValue(PS2AnalogStick::LY);
        uint8_t rx = ps2.getAnalogValue(PS2AnalogStick::RX);
        uint8_t ry = ps2.getAnalogValue(PS2AnalogStick::RY);
}