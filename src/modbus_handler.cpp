#include "modbus_handler.h"
#include "motor_control.h"

ModbusRTUServerClass RTUServer;

void setupModbus() {
    Serial.println("[Modbus] Starting Modbus RTU Server...");
    
    if (!RTUServer.begin(rs485, SLAVE_ID, MODBUS_BAUD, SERIAL_8N1)) {
        Serial.println("[Modbus] ERROR: Failed to start Modbus RTU Server!");
        return;
    } else {
        Serial.println("[Modbus] Modbus RTU Server started successfully");
        Serial.print("[Modbus] Slave ID: ");
        Serial.println(SLAVE_ID);
        Serial.print("[Modbus] Baud Rate: ");
        Serial.println(MODBUS_BAUD);
    }

    // Configure 6 holding registers
    RTUServer.configureHoldingRegisters(0, 6); 

    // Initialize registers
    RTUServer.holdingRegisterWrite(REG_RUN, 0);
    RTUServer.holdingRegisterWrite(REG_SPEED, motorVelocity);     // ใช้ค่าเริ่มต้นจาก motor_control
    RTUServer.holdingRegisterWrite(REG_TARGET, dispenseTarget);   // ใช้ค่าเริ่มต้นจาก motor_control
    RTUServer.holdingRegisterWrite(REG_STATUS, 0);
    RTUServer.holdingRegisterWrite(REG_COUNT, 0);
    RTUServer.holdingRegisterWrite(REG_PROGRESS, 0);
    
    Serial.println("[Modbus] Registers initialized");
    Serial.print("[Modbus] Initial Speed: ");
    Serial.println(motorVelocity);
    Serial.print("[Modbus] Initial Target: ");
    Serial.println(dispenseTarget);
}

void handleModbus() 
{
    // Poll for Modbus requests
    int pollResult = RTUServer.poll();
    
    // Debug: แสดงสถานะ poll อย่างสม่ำเสมอ (แต่ไม่บ่อยเกินไป)
    static unsigned long lastDebugTime = 0;
    if (millis() - lastDebugTime > 5000) {  // ทุก 5 วินาที
        lastDebugTime = millis();
        Serial.print("[Modbus Debug] Poll result: ");
        Serial.print(pollResult);
        Serial.print(", Motor running: ");
        Serial.print(motorRunning ? "YES" : "NO");
        Serial.print(", RUN register: ");
        Serial.println(RTUServer.holdingRegisterRead(REG_RUN));
    }
    
    if (pollResult) 
    {
        Serial.println("[Modbus] *** MODBUS REQUEST RECEIVED ***");
        
        int runCmd = RTUServer.holdingRegisterRead(REG_RUN);
        int speed  = RTUServer.holdingRegisterRead(REG_SPEED);
        int target = RTUServer.holdingRegisterRead(REG_TARGET);

        Serial.print("[Modbus] Received - RUN: ");
        Serial.print(runCmd);
        Serial.print(", SPEED: ");
        Serial.print(speed);
        Serial.print(", TARGET: ");
        Serial.println(target);

        // Update global variables from Modbus
        if (speed >= MOTOR_VELOCITY_MIN && speed <= MOTOR_VELOCITY_MAX) {
            motorVelocity = speed;
        }
        
        if (target >= DISPENSE_TARGET_MIN && target <= DISPENSE_TARGET_MAX) {
            dispenseTarget = target;
        }

        // Handle start/stop commands - ให้ทำงานทันทีไม่ต้องผ่าน state machine
        if (runCmd == 1 && !motorRunning) 
        {
            Serial.println("[Modbus] Starting motor DIRECTLY...");
            resetCounter();
            startMotor(MOTOR_HOMING_SPEED, true);   // ใช้ความเร็วคงที่เดียวกับ homing
            RTUServer.holdingRegisterWrite(REG_STATUS, 1);
            Serial.print("[Modbus] Motor START, Speed = ");
            Serial.println(MOTOR_HOMING_SPEED);
        } 
        else if (runCmd == 0 && motorRunning) 
        {
            Serial.println("[Modbus] Stopping motor...");
            stopMotor(false);
            RTUServer.holdingRegisterWrite(REG_STATUS, 0);
            Serial.println("[Modbus] Motor STOP");
        }
    }

    // Update read-only registers
    RTUServer.holdingRegisterWrite(REG_COUNT, objectCount & 0xFFFF);
    
    // Calculate and update progress percentage
    int progress = 0;
    if (dispenseTarget > 0) {
        progress = (objectCount * 100) / dispenseTarget;
        progress = constrain(progress, 0, 100);
    }
    RTUServer.holdingRegisterWrite(REG_PROGRESS, progress);
    
    // Update status based on motor state
    RTUServer.holdingRegisterWrite(REG_STATUS, motorRunning ? 1 : 0);
}
