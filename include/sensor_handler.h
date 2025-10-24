#ifndef SENSOR_HANDLER_H
#define SENSOR_HANDLER_H

#include <Arduino.h>
#include "system.h"

// Sensor configuration
#define SENSOR_MIN_INTERVAL_US 500    // Minimum time between sensor triggers (microseconds)

// Global variables
extern volatile unsigned long objectCount;      // Object counter (shared with motor_control)
extern volatile unsigned long sensorTriggerCount;  // Raw sensor trigger count

// Function declarations
void sensorInit();                    // Initialize sensor system
void sensorISR();                     // Sensor interrupt service routine
void handleSensorLogic();             // Process sensor logic (if needed)

#endif
