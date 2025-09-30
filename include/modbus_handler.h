
#ifndef MODBUS_HANDLER_H
#define MODBUS_HANDLER_H

#include <Arduino.h>
#include <ArduinoModbus.h>
#include "system.h"

// == Modbus settings ==
#define SLAVE_ID                    55  // Modbus Slave ID
#define REG_RUN                     0   // 0 = Stop, 1 = Run
#define REG_SPEED                   1   // PWM Speed
#define REG_BAUD_RATE               2   // Baud Rate (Default 9600) (R/W)
#define REG_IDENTIFIER              3   // Identifier (R/W, range 1-246, default 247)
#define REG_STATUS                  4   // 0 = Stop, 1 = Run (read only)
#define REG_COUNT                   5   // Object count register

#define COIL_NUM                    5000   // coils
#define DISCRETE_INPUT_NUM          1      // discrete inputs
#define HOLDING_REGISTER_NUM        400    // holding registers
#define INPUT_REGISTER_NUM          1      // input registers

// ModbusRTUServerClass object
extern ModbusRTUServerClass RTUServer;

void setupModbus();
void handleModbus(bool &motorRunning, int &pwmValue, unsigned long objectCount);

#endif
