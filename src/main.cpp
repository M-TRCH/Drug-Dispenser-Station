#include <Arduino.h>

#define RS232_RX_PIN PB7
#define RS232_TX_PIN PC4

HardwareSerial Serial232(RS232_RX_PIN, RS232_TX_PIN);  // ประกาศไว้ที่นี่ที่เดียว

void setup() {
  Serial.begin(115200);     // สำหรับดูใน Serial Monitor ผ่าน USB
  Serial232.begin(9600);    // สำหรับพอร์ต RS232 จริง
  Serial.println("=== RS232 Test Start ===");
}

void loop() {
  Serial232.println("Hello RS232");
  Serial.println("Sent: Hello RS232");
  delay(1000);
  
  // ถ้ามีข้อมูลกลับมาจาก RS232
  if (Serial232.available()) {
    String msg = Serial232.readStringUntil('\n');
    Serial.print("Received via RS232: ");
    Serial.println(msg);
  }
}