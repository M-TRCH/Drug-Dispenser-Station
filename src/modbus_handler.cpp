#include "modbus_handler.h"

ModbusRTUServerClass rtu;

// External variables from other modules
extern bool homePositionFound;  // From dispense.cpp
extern bool systemCalibrated;   // From main.cpp

// External functions from other modules
extern bool findHomePosition();        // From dispense.cpp
extern void returnToHome();            // From dispense.cpp
extern bool isAtTargetPosition(float target); // From dispense.cpp
extern float getCurrentPosition();      // From dispense.cpp
extern void resetCounter();            // From motor_control.cpp or dispense.cpp

// Internal state variables
static bool homingInProgress = false;
static bool calibrationInProgress = false;
static uint16_t currentErrorCode = ERR_NONE;
static uint32_t lastCommandTime = 0;

void modbusInit() 
{
    // Initialize Modbus RTU server
    if (!rtu.begin(rs485, MODBUS_SLAVE_ID, RS485_BAUDRATE, SERIAL_8N1)) {
        Serial.println("[Modbus] ERROR: Failed to start RTU Server!");
        return;
    }

    // Configure holding registers
    rtu.configureHoldingRegisters(0, MODBUS_REGISTER_COUNT);

    // Initialize register values
    rtu.holdingRegisterWrite(ADDR_REG_SPEED, MOTOR_PWM_DEFAULT);  // Default speed
    rtu.holdingRegisterWrite(ADDR_REG_DISP, 0);                  // No dispense command
    rtu.holdingRegisterWrite(ADDR_REG_HOME, HOME_CMD_NONE);       // No home command
    rtu.holdingRegisterWrite(ADDR_REG_CALIBRATE, CALIB_CMD_NONE); // No calibration command
    rtu.holdingRegisterWrite(ADDR_REG_STATUS, 0);                // Clear status
    rtu.holdingRegisterWrite(ADDR_REG_POSITION, 0);              // Position = 0
    rtu.holdingRegisterWrite(ADDR_REG_ERROR, ERR_NONE);          // No error

    // Reset internal state
    homingInProgress = false;
    calibrationInProgress = false;
    currentErrorCode = ERR_NONE;

    Serial.printf("[Modbus] RTU Server started (Slave ID: %d)\n", MODBUS_SLAVE_ID);
    Serial.println("[Modbus] Extended Register Map:");
    Serial.printf("  - Register %d: Motor Speed (0-4095)\n", ADDR_REG_SPEED);
    Serial.printf("  - Register %d: Dispense Command (1-99 rotations)\n", ADDR_REG_DISP);
    Serial.printf("  - Register %d: Home Command (1=find, 2=return)\n", ADDR_REG_HOME);
    Serial.printf("  - Register %d: Calibration Command (1=start)\n", ADDR_REG_CALIBRATE);
    Serial.printf("  - Register %d: System Status (read-only)\n", ADDR_REG_STATUS);
    Serial.printf("  - Register %d: Current Position x100 (read-only)\n", ADDR_REG_POSITION);
    Serial.printf("  - Register %d: Error Code (read-only)\n", ADDR_REG_ERROR);
}

void modbusHandler()
{
    // Update homing process if in progress
    updateHomingProcess();
    
    // Process Modbus communication
    if (rtu.poll()) {
        // Read command registers
        int speed = (int)rtu.holdingRegisterRead(ADDR_REG_SPEED);
        int dispense = (int)rtu.holdingRegisterRead(ADDR_REG_DISP);
        int homeCmd = (int)rtu.holdingRegisterRead(ADDR_REG_HOME);
        int calibCmd = (int)rtu.holdingRegisterRead(ADDR_REG_CALIBRATE);

        // Process home commands
        if (homeCmd != HOME_CMD_NONE) {
            processHomeCommand(homeCmd);
            rtu.holdingRegisterWrite(ADDR_REG_HOME, HOME_CMD_NONE); // Clear command
        }

        // Process calibration commands  
        if (calibCmd != CALIB_CMD_NONE) {
            processCalibrationCommand(calibCmd);
            rtu.holdingRegisterWrite(ADDR_REG_CALIBRATE, CALIB_CMD_NONE); // Clear command
        }

        // Process dispense command (only if system is ready)
        if (speed > 0 && dispense > 0) {
            if (homePositionFound) {  // Only require home position, not full calibration
                // Validate inputs
                speed = constrain(speed, MOTOR_PWM_MIN, MOTOR_PWM_MAX);
                dispense = constrain(dispense, 1, DISPENSE_MAX_ROTATIONS);
                
                Serial.printf("[Modbus] Dispense command: %d rotations at speed %d\n", dispense, speed);
                
                // Start dispensing
                dispense_start(dispense, speed);
                
                // Clear dispense command after execution
                rtu.holdingRegisterWrite(ADDR_REG_DISP, 0);
                currentErrorCode = ERR_NONE;
            } else {
                // System not ready - set error
                currentErrorCode = ERR_HOME_NOT_FOUND;
                Serial.println("[Modbus] ERROR: Home position not found. Run homing first.");
                rtu.holdingRegisterWrite(ADDR_REG_DISP, 0); // Clear invalid command
            }
        }
        
        // Update all status registers
        updateStatusRegisters();
    }
}

void processHomeCommand(int homeCmd)
{
    switch (homeCmd) {
        case HOME_CMD_FIND:
            if (!homingInProgress && !dispenseActive) {
                Serial.println("[Modbus] Starting home position search...");
                homingInProgress = true;
                currentErrorCode = ERR_NONE;
                lastCommandTime = millis();
                
                // Start homing process (similar to commit version)
                resetCounter();
                startMotor(MOTOR_PWM_SLOW, true);  // Use slow speed for homing
                
                Serial.println("[Modbus] Homing started - motor running at slow speed");
            } else {
                currentErrorCode = ERR_INVALID_COMMAND;
                Serial.println("[Modbus] Cannot start homing - system busy");
            }
            break;
            
        case HOME_CMD_RETURN:
            if (!homingInProgress && !dispenseActive && homePositionFound) {
                Serial.println("[Modbus] Returning to home position...");
                returnToHome();
                currentErrorCode = ERR_NONE;
            } else {
                currentErrorCode = ERR_INVALID_COMMAND;
                Serial.println("[Modbus] Cannot return home - system not ready");
            }
            break;
            
        default:
            currentErrorCode = ERR_INVALID_COMMAND;
            break;
    }
}

// ===== HOME DETECTION LOGIC =====
void updateHomingProcess()
{
    if (!homingInProgress) return;
    
    // Check if home sensor (SEN_2) is triggered
    if (digitalRead(SEN_2_PIN) == LOW) {  // Assuming active LOW
        Serial.println("[Modbus] Home sensor triggered!");
        
        // Stop motor
        stopMotor(false);
        delay(200);  // Allow motor to settle
        
        // Set home position
        resetCounter();
        homePositionFound = true;
        homingInProgress = false;
        currentErrorCode = ERR_NONE;
        
        Serial.println("[Modbus] Home position found and set!");
        
        // Optional: Start dispensing immediately after homing
        delay(500);
        Serial.println("[Modbus] Starting normal operation after homing...");
        startMotor(MOTOR_PWM_DEFAULT, true);
        
    } else {
        // Check timeout
        if (millis() - lastCommandTime > 30000) {  // 30 second timeout
            Serial.println("[Modbus] Homing timeout!");
            stopMotor(false);
            homingInProgress = false;
            currentErrorCode = ERR_HOME_NOT_FOUND;
        }
    }
}

void processCalibrationCommand(int calibCmd)
{
    switch (calibCmd) {
        case CALIB_CMD_START:
            if (!calibrationInProgress && !dispenseActive && !homingInProgress) {
                Serial.println("[Modbus] Starting system calibration...");
                calibrationInProgress = true;
                currentErrorCode = ERR_NONE;
                
                // Start calibration (this should be non-blocking)
                performSystemCalibration();
                
                calibrationInProgress = false;
                Serial.println("[Modbus] Calibration completed");
            } else {
                currentErrorCode = ERR_INVALID_COMMAND;
                Serial.println("[Modbus] Cannot start calibration - system busy");
            }
            break;
            
        default:
            currentErrorCode = ERR_INVALID_COMMAND;
            break;
    }
}

void updateStatusRegisters()
{
    // Update status register with bit flags
    uint16_t status = 0;
    if (flag_motorRunning) status |= STATUS_MOTOR_RUNNING;
    if (dispenseActive) status |= STATUS_DISPENSE_ACTIVE;
    if (homePositionFound) status |= STATUS_HOME_FOUND;
    if (systemCalibrated) status |= STATUS_CALIBRATED;
    if (homingInProgress) status |= STATUS_HOMING;
    if (calibrationInProgress) status |= STATUS_CALIBRATING;
    if (currentErrorCode != ERR_NONE) status |= STATUS_ERROR;
    
    // Check if at home position (within tolerance)
    if (homePositionFound && isAtTargetPosition(0.0)) {
        status |= STATUS_AT_HOME;
    }
    
    rtu.holdingRegisterWrite(ADDR_REG_STATUS, status);
    
    // Update position register (scaled by 100 for precision)
    int16_t position = (int16_t)(getCurrentPosition() * 100);
    rtu.holdingRegisterWrite(ADDR_REG_POSITION, position);
    
    // Update error register
    rtu.holdingRegisterWrite(ADDR_REG_ERROR, currentErrorCode);
}
