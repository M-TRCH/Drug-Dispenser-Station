#include <Arduino.h>

#define RS232_RX_PIN PB7
#define RS232_TX_PIN PC4

HardwareSerial Serial232(RS232_RX_PIN, RS232_TX_PIN);

void setup() {
  Serial.begin(115200);     // สำหรับดูใน Serial Monitor ผ่าน USB
  Serial232.begin(9600);    // สำหรับพอร์ต RS232 จริง
  Serial.println("=== RS232 Test Start ===");
}

void loop() {
  // ส่งข้อมูลออกทาง RS232
  Serial232.println("Hello from STM32 RS232");

  // แสดงข้อความใน Serial Monitor ด้วย
  Serial.println("Sent: Hello from STM32 RS232");
  delay(1000);

  // ถ้ามีข้อมูลกลับมาจาก RS232 (เช่นจาก loopback หรือ PC)
  if (Serial232.available()) {
    String msg = Serial232.readStringUntil('\n');
    Serial.print("Received via RS232: ");
    Serial.println(msg);
  }
}