#ifndef SENSOR_HANDLER_H
#define SENSOR_HANDLER_H

#include <Arduino.h>

#define SENSOR_MIN_INTERVAL_US 500

extern volatile unsigned long objectCount;

void sensorInit();
void sensorISR();
void handleSensorLogic();

#endif
