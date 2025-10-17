
#ifndef MODBUS_HANDLER_H
#define MODBUS_HANDLER_H

#include <Arduino.h>
#include <ArduinoModbus.h>
#include "system.h"

// == Modbus settings ==
#define REG_RUN         0   // 0=Stop, 1=Run
#define REG_SPEED       1   // PWM Speed (500-4000)
#define REG_TARGET      2   // Target count (1-9999)
#define REG_STATUS      3   // 0=Stopped, 1=Running
#define REG_COUNT       4   // Current object count (read-only)
#define REG_PROGRESS    5   // Progress percentage (read-only)

#define SLAVE_ID        1

// ModbusRTUServerClass object
extern ModbusRTUServerClass RTUServer;

void setupModbus();
void handleModbus();

#endif
