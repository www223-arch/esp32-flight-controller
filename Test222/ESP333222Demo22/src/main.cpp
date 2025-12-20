#include <Arduino.h>

/*
  Uses a for loop to print numbers in various formats.
*/
// Arduino Mega using all four of its Serial ports
// (Serial, Serial1, Serial2, Serial3),
// with different baud rates:

void setup() {
    pinMode(32, OUTPUT);  // 板载LED引脚
  Serial.begin(115200);

 delay(1000);  // 关键：等待串口初始化完成
  Serial.println("Hello Computer");

}

void loop() {

  Serial.println("Hello Computer");

   digitalWrite(32, HIGH);
  delay(500);
  digitalWrite(32, LOW);
  delay(500);
  Serial.println("Hello Computer");
}