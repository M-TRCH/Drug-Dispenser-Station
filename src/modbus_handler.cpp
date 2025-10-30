#include "modbus_handler.h"

ModbusRTUServerClass rtu;

// External variables from other modules
extern bool homePositionFound;  // From dispense.cpp

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
    rtu.holdingRegisterWrite(ADDR_REG_STATUS, 0);                // Clear status

    Serial.printf("[Modbus] RTU Server started (Slave ID: %d)\n", MODBUS_SLAVE_ID);
    Serial.println("[Modbus] Register Map:");
    Serial.printf("  - Register %d: Motor Speed (0-4095)\n", ADDR_REG_SPEED);
    Serial.printf("  - Register %d: Dispense Command (1-99 rotations)\n", ADDR_REG_DISP);
    Serial.printf("  - Register %d: System Status (read-only)\n", ADDR_REG_STATUS);
}

void modbusHandler()
{
    // Process Modbus communication
    if (rtu.poll()) {
        // Read command registers
        int speed = (int)rtu.holdingRegisterRead(ADDR_REG_SPEED);
        int dispense = (int)rtu.holdingRegisterRead(ADDR_REG_DISP);

        // Process dispense command
        if (speed > 0 && dispense > 0) {
            // Validate inputs
            speed = constrain(speed, MOTOR_PWM_MIN, MOTOR_PWM_MAX);
            dispense = constrain(dispense, 1, DISPENSE_MAX_ROTATIONS);
            
            Serial.printf("[Modbus] Command received: %d rotations at speed %d\n", dispense, speed);
            
            // Start dispensing
            dispense_start(dispense, speed);
            
            // Clear dispense command after execution
            rtu.holdingRegisterWrite(ADDR_REG_DISP, 0);
        }
        
        // Update status register
        uint16_t status = 0;
        if (flag_motorRunning) status |= STATUS_MOTOR_RUNNING;
        if (dispenseActive) status |= STATUS_DISPENSE_ACTIVE;
        if (homePositionFound) status |= STATUS_HOME_FOUND;
        // Note: systemCalibrated would need to be extern if we want to include it
        
        rtu.holdingRegisterWrite(ADDR_REG_STATUS, status);
    }
}
