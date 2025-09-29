
#ifndef MODBUS_HANDLER_H
#define MODBUS_HANDLER_H

#include <Arduino.h>
#include <ArduinoModbus.h>
#include "system.h"

#define SLAVE_ID                    55  // Modbus Slave ID
#define REG_RUN                     0   // 0 = Stop, 1 = Run
#define REG_SPEED                   1   // PWM Speed
#define REG_BAUD_RATE               2   // baud rate (default 9600) (read/write)
#define REG_IDENTIFIER              3   // identifier (read/write, range 1-246, default 247)
#define REG_STATUS                  4   // 0 = Stop, 1 = Run (read only)
#define REG_COUNT                   5   // object count register

#define COIL_NUM                    5000 // Number of coils
#define DISCRETE_INPUT_NUM          1   // Number of discrete inputs
#define HOLDING_REGISTER_NUM        400 // Number of holding registers
#define INPUT_REGISTER_NUM          1   // Number of input registers

extern ModbusRTUServerClass RTUServer;

void setupModbus();
void handleModbus(bool &motorRunning, int &pwmValue, unsigned long objectCount);
void modbusInit(int id=SLAVE_ID);

#endif
