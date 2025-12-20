#include <WiFi.h>
#include <WiFiUdp.h>

// 替换成你的手机热点SSID和密码
const char* ssid = "223Wl";
const char* password = "12345678";

WiFiUDP udp;
unsigned int localPort = 8888; // ESP32监听的端口（和手机端一致）
IPAddress phoneIP(10, 192, 108, 27); // 替换成你的手机IP（热点网关IP）
unsigned int phonePort = 8888; // 和手机端一致

// 模拟无人机飞控数据（你可以替换成真实的IMU/电压数据）
float pitch = 0.0;  // 俯仰角
float roll = 0.0;   // 横滚角
float batVolt = 11.1; // 电池电压
int throttle = 0;   // 油门值（接收手机指令更新）

void setup() {
  Serial.begin(115200);
  
  // 连接手机热点
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" 连接手机热点成功！");
  Serial.print("ESP32 IP：");
  Serial.println(WiFi.localIP()); // 重点：记录这个IP
  Serial.print("手机热点网关IP（手机IP）：");
  Serial.println(phoneIP);

  // 初始化UDP
  udp.begin(localPort);
  Serial.println(" UDP初始化完成，监听端口：" + String(localPort));
}

void loop() {
  // 1. 接收手机发送的控制指令（比如"throttle:50" "pitch:2.5"）
  int packetSize = udp.parsePacket(); // 检测是否有数据到来
  if (packetSize > 0) {
    char cmdBuf[128] = {0};
    udp.read(cmdBuf, packetSize); // 读取手机发来的指令
    Serial.print("收到手机指令：");
    Serial.println(cmdBuf);

    // 解析指令（示例：解析油门值）
    String cmd = String(cmdBuf);
    if (cmd.startsWith("throttle:")) {
      throttle = cmd.substring(9).toInt(); // 提取"throttle:50"中的50
      Serial.print(" 油门值更新为：");
      Serial.println(throttle);
    }
  }

  // 2. 向手机回传飞控数据（每秒10次，实时性高）
  static unsigned long lastSend = 0;
  if (millis() - lastSend > 100) { // 100ms发送一次（10Hz）
    lastSend = millis();
    // 组装数据格式："pitch:0.0,roll:0.0,bat:11.1,throttle:0"
    String data = "pitch:" + String(pitch, 1) + 
                  ",roll:" + String(roll, 1) + 
                  ",bat:" + String(batVolt, 1) + 
                  ",throttle:" + String(throttle);
    
    // 发送数据到手机
    udp.beginPacket(phoneIP, phonePort);
    udp.print(data);
    udp.endPacket();

    Serial.print(" 向手机回传数据：");
    Serial.println(data);

    // 模拟姿态变化（测试用，实际替换成IMU数据）
    pitch += 0.1;
    roll -= 0.1;
    if (pitch > 5.0) pitch = 0.0;
    if (roll < -5.0) roll = 0.0;
  }
}