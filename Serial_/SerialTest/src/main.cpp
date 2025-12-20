#include <WiFi.h>
// 必须和手机热点的SSID、密码完全一致（大小写、字符都不能错）
const char* ssid = "223Wl";
const char* password = "12345678";

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA); // 明确设置为STA模式（连接热点的模式）
  WiFi.begin(ssid, password);
  Serial.print("正在连接手机热点...");

  // 等待连接，同时打印状态（关键：看失败原因）
  unsigned long startMillis = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    // 超时处理（避免死循环）
    if (millis() - startMillis > 15000) { // 15秒连不上就报错
      Serial.println("\n连接超时！错误码：" + String(WiFi.status()));
      break;
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n连接成功！ESP32 IP：" + WiFi.localIP().toString());
  }
}

void loop() {
  // 持续检查连接状态
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("连接断开，重新连接中...");
    WiFi.reconnect();
  }
  else
  {
    Serial.println("已连接，IP地址：" + WiFi.localIP().toString());
  }
  delay(1000);
}