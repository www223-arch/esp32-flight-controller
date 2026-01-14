#include <Arduino.h>
const int testGPIO = 4;
bool isInited = false;

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  if (!isInited) {
    pinMode(testGPIO, OUTPUT);
    // 初始化信息：明确、无多余字符
    Serial.println("=====================");
    Serial.println(" 初始化完成：GPIO4已配置");
    Serial.println("=====================");
    isInited = true;
  }
}

void loop() {
  // 高电平状态：清晰标注GPIO、状态、时间间隔
  digitalWrite(testGPIO, HIGH);
  Serial.print("GPIO");
  Serial.print(testGPIO);
  Serial.println(" | 状态：高电平 | 持续200ms");
  delay(200);

  // 低电平状态：格式与高电平统一
  digitalWrite(testGPIO, LOW);
  Serial.print("GPIO");
  Serial.print(testGPIO);
  Serial.println(" | 状态：低电平 | 持续100ms");
  Serial.println("---------------------"); // 分隔线，区分每次状态切换
  delay(100);
}