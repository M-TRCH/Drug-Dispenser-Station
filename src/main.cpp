#include <Arduino.h>

#define RS485_RX3_PIN PB11
#define RS485_TX3_PIN PB10

HardwareSerial Serial3(RS485_RX3_PIN, RS485_TX3_PIN);  // ประกาศไว้ที่นี่ที่เดียว

void setup() {
  Serial3.begin(9600);
}

void loop() {
  Serial3.println("Hello RS485");
  delay(1000);
}
