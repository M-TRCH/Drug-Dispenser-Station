#ifndef MODBUS_HANDLER_H
#define MODBUS_HANDLER_H

#include <Arduino.h>
#include <ArduinoModbus.h>
#include <ArduinoRS485.h>

#define SLAVE_ID      55
#define REG_RUN       0
#define REG_SPEED     1
#define REG_STATUS    2
#define REG_COUNT     3

extern ModbusRTUServerClass RTUServer;

void setupModbus();
void handleModbus();

#endif
