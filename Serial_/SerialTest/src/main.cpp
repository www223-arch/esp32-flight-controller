#include <WiFi.h>
#include "ps2.h"
// 创建PS2控制器实例（可使用默认引脚或自定义引脚）
PS2Controller ps2;  // 默认引脚
// PS2Controller ps2(GPIO_NUM_2, GPIO_NUM_4, GPIO_NUM_5, GPIO_NUM_18);  // 自定义引脚

// ******** 替换成你的手机热点信息 ********
const char* ssid = "223Wl";    // 手机热点名称（纯英文/数字，无特殊字符）
const char* password = "12345678";       // 手机热点密码（≥8位）
// **************************************

const uint16_t tcpPort = 8888;           // TCP通信端口（手机端必须和这个一致）
WiFiServer server(tcpPort);              // 创建TCP服务器
WiFiClient client;                       // 存储连接的手机客户端

// 模拟无人机飞控数据（可替换为真实的IMU/电压数据）
float pitch = 0.0;   // 俯仰角
float roll = 0.0;    // 横滚角
float batVolt = 11.1;// 电池电压
int throttle = 0;    // 油门值（接收手机指令更新）

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