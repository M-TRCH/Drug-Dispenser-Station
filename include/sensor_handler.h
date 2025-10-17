#pragma once
#include <Arduino.h>

#define SENSOR_MIN_INTERVAL_US 500

extern volatile unsigned long objectCount;
void sensorInit();
void sensorISR(); // เพิ่ม declaration สำหรับ ISR
void handleSensorLogic();
