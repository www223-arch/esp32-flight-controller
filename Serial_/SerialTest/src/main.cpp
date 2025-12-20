#include <WiFi.h>

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
  
  // 1. 连接手机热点
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("正在连接手机热点...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // 连接成功，打印关键信息（重点记录ESP32的IP！）
  Serial.println("\n? 手机热点连接成功！");
  Serial.print("ESP32的IP地址（手机端需要填这个）：");
  Serial.println(WiFi.localIP()); // 例：192.168.43.123（安卓）/172.20.10.5（iPhone）
  Serial.print("TCP服务器端口：");
  Serial.println(tcpPort);

  // 2. 启动TCP服务器，等待手机连接
  server.begin();
  server.setTimeout(1000); // 超时时间1秒
  Serial.println("? TCP服务器已启动，等待手机客户端连接...");
}

void loop() {
  // 3. 检测并接受手机的连接请求
  if (!client.connected()) {
    client = server.available(); // 等待手机连接
    if (client) {
      Serial.println("\n? 手机已成功连接TCP服务器！");
      // 给手机发送欢迎消息
      client.println("Hello! ESP32飞控已连接，可发送指令（例：throttle:50）");
    }
    return; // 未连接时，跳过后续通信逻辑
  }

  // 4. 接收手机发送的控制指令（可靠传输，不丢包）
  if (client.available() > 0) {
    String cmd = client.readStringUntil('\n'); // 读取指令（换行符结束）
    cmd.trim(); // 去除空格/换行符
    if (cmd.length() > 0) {
      Serial.print("? 收到手机指令：");
      Serial.println(cmd);

      // 解析指令（示例：解析油门/俯仰角）
      if (cmd.startsWith("throttle:")) {
        throttle = cmd.substring(9).toInt(); // 提取"throttle:50"中的50
        Serial.print("? 油门值更新为：");
        Serial.println(throttle);
      }
      if (cmd.startsWith("pitch:")) {
        pitch = cmd.substring(6).toFloat(); // 提取"pitch:2.5"中的2.5
        Serial.print("? 俯仰角更新为：");
        Serial.println(pitch);
      }
    }
  }

  // 5. 向手机回传飞控数据（每秒10次，实时且可靠）
  static unsigned long lastSend = 0;
  if (millis() - lastSend > 100) { // 100ms发送一次（10Hz）
    lastSend = millis();
    // 组装数据格式（键值对，易解析）
    String data = "pitch:" + String(pitch, 1) + 
                  ",roll:" + String(roll, 1) + 
                  ",bat:" + String(batVolt, 1) + 
                  ",throttle:" + String(throttle);
    
    // 发送数据到手机（TCP确保数据送达）
    client.println(data);
    Serial.print("? 向手机回传数据：");
    Serial.println(data);

    // 模拟姿态变化（测试用，实际替换为IMU读取）
    roll -= 0.1;
    if (roll < -5.0) roll = 0.0;
  }

  // 6. 检测连接是否断开（断开后重新等待连接）
  if (!client.connected()) {
    Serial.println("? 手机已断开连接，等待重新连接...");
    client.stop(); // 释放客户端资源
  }
}