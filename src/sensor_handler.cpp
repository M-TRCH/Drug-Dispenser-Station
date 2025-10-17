#include "sensor_handler.h"
#include "motor_control.h"
#include "system.h"

volatile unsigned long sensorTriggerCount = 0;  // นับสัญญาณดิบจาก FS-N11N
// volatile unsigned long objectCount = 0;      // ลบออกเพราะ define อยู่ใน motor_control.cpp แล้ว
unsigned long lastTriggerTime = 0;

void sensorInit() {
  pinMode(SEN_1_PIN, INPUT_PULLUP); // FS-N11N เป็น NPN
  attachInterrupt(digitalPinToInterrupt(SEN_1_PIN), sensorISR, FALLING);
  Serial.println("[Sensor] FS-N11N Initialized (NPN, FALLING edge)");
}

void sensorISR() {
  unsigned long now = micros();
  if (now - lastTriggerTime > SENSOR_MIN_INTERVAL_US) {
    lastTriggerTime = now;
    sensorTriggerCount++;

    // นับรอบจริงทุก 2 ครั้ง
    if (sensorTriggerCount % 2 == 0) {
      objectCount++;
      Serial.printf("[Sensor] Round detected: %lu\n", objectCount);
    }
  }
}

void handleSensorLogic() {
  // ไม่ต้องทำอะไรเพิ่ม ISR จัดการแล้ว
}
