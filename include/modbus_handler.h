#ifndef MODBUS_HANDLER_H
#define MODBUS_HANDLER_H

#include <Arduino.h>
#include <ArduinoModbus.h>
#include "motor_control.h"

// Modbus configuration
#define MODBUS_SLAVE_ID         55        // Modbus slave address
#define MODBUS_REGISTER_COUNT   100       // Number of holding registers

// Modbus register map
#define ADDR_REG_SPEED          10      // Speed setpoint register
#define ADDR_REG_DISP           11      // Dispense command register
#define ADDR_REG_STATUS         12      // Status register

// Global objects
extern ModbusRTUServerClass rtu;    // Modbus RTU server instance

// Function declarations
void modbusInit();                       // Initialize Modbus communication
void modbusHandler();                    // Process Modbus communication

#endif
