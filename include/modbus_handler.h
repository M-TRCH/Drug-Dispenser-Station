#ifndef MODBUS_HANDLER_H
#define MODBUS_HANDLER_H

#include <Arduino.h>
#include <ArduinoModbus.h>
#include "motor_control.h"
#include "dispense.h"

// Modbus configuration
#define MODBUS_SLAVE_ID         55        // Modbus slave address
#define MODBUS_REGISTER_COUNT   50        // Number of holding registers

// Modbus register map (Simple - only 3 registers)
#define ADDR_REG_SPEED          10      // Speed setpoint register (0-4095)
#define ADDR_REG_DISP           11      // Dispense command register (1-99 rotations)
#define ADDR_REG_STATUS         12      // Status register (read-only)

// Status bit flags
#define STATUS_MOTOR_RUNNING    (1 << 0)   // Motor is running
#define STATUS_DISPENSE_ACTIVE  (1 << 1)   // Dispense operation active
#define STATUS_HOME_FOUND       (1 << 2)   // Home position found
#define STATUS_CALIBRATED       (1 << 3)   // System calibrated

// Global objects
extern ModbusRTUServerClass rtu;    // Modbus RTU server instance

// Function declarations
void modbusInit();                       // Initialize Modbus communication
void modbusHandler();                    // Process Modbus communication

#endif
