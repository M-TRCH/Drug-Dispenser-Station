#ifndef MODBUS_HANDLER_H
#define MODBUS_HANDLER_H

#include <Arduino.h>
#include <ArduinoModbus.h>
#include <ArduinoRS485.h>
#include "system.h"

// Modbus configuration
#define MODBUS_SLAVE_ID         55        // Modbus slave address
#define MODBUS_REGISTER_COUNT   8         // Number of holding registers

// Modbus register map
#define REG_RUN                 0         // Run command register
#define REG_SPEED               1         // Speed setpoint register  
#define REG_STATUS              2         // Status register
#define REG_COUNT               3         // Object count register
#define REG_TARGET              4         // Dispense target register
#define REG_RESET               5         // Reset command register

// Global objects
extern ModbusRTUServerClass RTUServer;    // Modbus RTU server instance

// Function declarations
void setupModbus();                       // Initialize Modbus communication
void handleModbus();                      // Process Modbus communication

#endif
