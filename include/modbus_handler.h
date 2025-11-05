#ifndef MODBUS_HANDLER_H
#define MODBUS_HANDLER_H

#include <Arduino.h>
#include <ArduinoModbus.h>
#include "motor_control.h"
#include "dispense.h"

// Modbus configuration
#define MODBUS_SLAVE_ID         55        // Modbus slave address
#define MODBUS_REGISTER_COUNT   50        // Number of holding registers

// Modbus register map (Extended with Homing) 
#define ADDR_REG_SPEED          10      // Speed setpoint register (0-4095)
#define ADDR_REG_DISP           11      // Dispense command register (1-99 rotations)
#define ADDR_REG_HOME           12      // Home command register (1=find home, 2=return home)

// Status registers (read-only)
#define ADDR_REG_STATUS         20      // Status register (read-only)
#define ADDR_REG_POSITION       21      // Current position (scaled x100, read-only)
#define ADDR_REG_ERROR          22      // Error code register (read-only)

// Home command codes
#define HOME_CMD_NONE           0       // No home command
#define HOME_CMD_FIND           1       // Find home position
#define HOME_CMD_RETURN         2       // Return to home position

// Calibration command codes  
//#define CALIB_CMD_NONE          0       // No calibration command
//#define CALIB_CMD_START         1       // Start calibration

// Sensor test command codes
//#define SENSOR_TEST_NONE        0       // No sensor test
//#define SENSOR_TEST_START       1       // Start sensor test (10 readings)

// Status bit flags
#define STATUS_MOTOR_RUNNING    (1 << 0)   // Motor is running
#define STATUS_DISPENSE_ACTIVE  (1 << 1)   // Dispense operation active
#define STATUS_HOME_FOUND       (1 << 2)   // Home position found
#define STATUS_CALIBRATED       (1 << 3)   // System calibrated
#define STATUS_AT_HOME          (1 << 4)   // Currently at home position
#define STATUS_HOMING           (1 << 5)   // Homing operation in progress
#define STATUS_CALIBRATING      (1 << 6)   // Calibration in progress
#define STATUS_ERROR            (1 << 7)   // Error condition

// Error codes
#define ERR_NONE                0       // No error
#define ERR_HOME_NOT_FOUND      1       // Cannot find home position
#define ERR_NOT_CALIBRATED      2       // System not calibrated
#define ERR_INVALID_COMMAND     3       // Invalid command received
#define ERR_MOTOR_TIMEOUT       4       // Motor operation timeout

// Global objects
extern ModbusRTUServerClass rtu;    // Modbus RTU server instance

// ===== SHARED VARIABLES =====
extern bool isEnhancedHomingActive;  // แชร์สถานะ Enhanced Homing กับไฟล์อื่น

// Function declarations
void modbusInit();                       // Initialize Modbus communication
void modbusHandler();                    // Process Modbus communication
void processHomeCommand(int homeCmd);    // Process home commands
void processDispenseCommand(int dispenseRotations); // Process dispense commands
void processCalibrationCommand(int calibCmd); // Process calibration commands
void processSensorTestCommand(int testCmd);   // Process sensor test commands
void updateStatusRegisters();            // Update status registers
void updateHomingProcess();              // Update homing process (non-blocking)

// ===== PHASE 1: ENHANCED HOME DETECTION =====
void enhancedHomeDetection();            // Enhanced home position detection
void confirmHomePosition();              // Confirm and set home position

// External function declarations (from main.cpp)
extern void performSystemCalibration();  // System calibration function

// External function declarations (from dispense.cpp)
extern void disableRotationISR();        // Disable rotation counting ISR
extern void enableRotationISR();         // Enable rotation counting ISR

#endif

