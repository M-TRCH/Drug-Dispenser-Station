#ifndef SENSOR_HANDLER_H
#define SENSOR_HANDLER_H

#include <Arduino.h>

// === Sensor Pin & Timing ===
#define SEN_1_PIN             PB9      // กำหนดขาอินพุตที่ต่อกับ FS-N11N
#define SENSOR_ACTIVE_STATE    LOW      // NPN → LOW เมื่อเจอวัตถุ
#define SENSOR_DEBOUNCE_TIME_MS 30      // ป้องกัน bounce

void sensorInit();       // ตั้งค่า pin และ interrupt
void sensorISR();        // ฟังก์ชัน interrupt handler
void handleSensorLogic(); // ใช้ใน loop() ถ้าต้องการเช็คใน polling mode

#endif
