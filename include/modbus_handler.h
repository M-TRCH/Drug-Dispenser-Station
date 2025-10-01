
#ifndef MODBUS_HANDLER_H
#define MODBUS_HANDLER_H

#include <Arduino.h>
#include <ArduinoModbus.h>
#include "system.h"

// == Modbus settings ==
#define REG_RUN         0   // 0=Stop, 1=Run
#define REG_SPEED       1   // PWM Speed
#define REG_STATUS      2   // 0=Stopped, 1=Running
#define REG_COUNT       3   // Object counter

#define SLAVE_ID        1

// ModbusRTUServerClass object
extern ModbusRTUServerClass RTUServer;

void setupModbus();
void handleModbus(unsigned long objectCount);

#endif
