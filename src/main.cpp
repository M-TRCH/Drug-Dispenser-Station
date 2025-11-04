/**
 * @file main.cpp
 * @brief Drug Dispenser Station - Enhanced Home Detection System
 * @date November 2025
 *
 * Description:
 * - Phase 1 Enhanced Home Detection with validation
 * - Sensor Health Monitoring system
 * - Modbus RTU commands: Speed(10), Dispense(11), Home(12)
 * - Enhanced home detection requires 3 consecutive detections
 * - Use Register 12 = 1 to start Enhanced Home Detection
 */

#include "system.h"
#include "motor_control.h"
#include "dispense.h"
#include "modbus_handler.h"

// Global system status variables
bool systemCalibrated = false;
bool systemReady = false;

void setup() 
{
#ifdef SYSTEM_H
    systemInit();        // Core system initialization
#endif

#ifdef MOTOR_CONTROL_H
    motorControlInit();  // Motor control system
#endif

#ifdef DISPENSE_H
    dispenseInit();      // Dispense system initialization
#endif
    
#ifdef MODBUS_HANDLER_H
    modbusInit();        // Modbus communication initialization
#endif

    Serial.println("\n==================================================");
    Serial.println("DRUG DISPENSER - ENHANCED HOME DETECTION READY");
    Serial.println("==================================================");
    Serial.println("[Main] ระบบเริ่มต้นสำเร็จ");
    Serial.println("[Main] Phase 1: Enhanced Home Detection + Sensor Health");
    Serial.println("");
    Serial.println("Modbus Commands:");
    Serial.println("   Register 12 = 1 -> เริ่ม Enhanced Home Detection");
    Serial.println("   Register 12 = 2 -> Return to Home");  
    Serial.println("   Register 14 = 1 -> ทดสอบเซ็นเซอร์ (10 ครั้ง)");
    Serial.println("   Register 20-22 -> อ่านสถานะระบบ");
    Serial.println("");
    Serial.println("Features:");
    Serial.println("   + Enhanced Detection (3 ครั้งต่อเนื่อง)");
    Serial.println("   + Sensor Health Monitoring");
    Serial.println("   + ISR Control สำหรับป้องกัน conflict");
    Serial.println("==================================================\n");
    
    // เพิ่มการทดสอบเซ็นเซอร์เบื้องต้น
    Serial.println("[Sensor Test] ทดสอบเซ็นเซอร์เบื้องต้น:");
    for (int i = 0; i < 5; i++) {
        bool sensorState = (digitalRead(SEN_1_PIN) == LOW);
        Serial.printf("[Sensor Test] ครั้งที่ %d: SEN_1 = %s\n", 
                     i+1, sensorState ? "LOW (ตรวจจับ)" : "HIGH (ไม่ตรวจจับ)");
        delay(200);
    }
    Serial.println("");
    
    // ไม่ทำ Auto Homing - ให้ใช้ Enhanced Detection ผ่าน Modbus แทน
}

void loop() 
{
    // ===== SYSTEM MAINTENANCE =====
    dispense_update();      // Update dispense system (safety checks + sensor health)
    updateHomingProcess();  // Update enhanced homing process if in progress
    
    // ===== SENSOR STATUS MONITORING =====
    static unsigned long lastSensorCheck = 0;
    if (millis() - lastSensorCheck > 3000) {  // ตรวจสอบทุก 3 วินาที
        bool sensorState = (digitalRead(SEN_1_PIN) == LOW);
        Serial.printf("[Sensor Monitor] SEN_1: %s | Enhanced Homing: %s | Motor: %s\n", 
                     sensorState ? "LOW" : "HIGH",
                     isEnhancedHomingActive ? "ACTIVE" : "INACTIVE",
                     flag_motorRunning ? "RUNNING" : "STOPPED");
        lastSensorCheck = millis();
    }
    
    // ===== MODBUS COMMAND PROCESSING =====
    if (rtu.poll())
    {
        int speed = (int)rtu.holdingRegisterRead(ADDR_REG_SPEED);
        int dispense = (int)rtu.holdingRegisterRead(ADDR_REG_DISP);
        int homeCmd = (int)rtu.holdingRegisterRead(ADDR_REG_HOME);
        int calibCmd = (int)rtu.holdingRegisterRead(ADDR_REG_CALIBRATE);
        int sensorTestCmd = (int)rtu.holdingRegisterRead(ADDR_REG_SENSOR_TEST);  // เพิ่ม sensor test
    
        // Debug: แสดงเฉพาะเมื่อมี command จริง ๆ
        if (speed > 0 || dispense > 0 || homeCmd > 0 || calibCmd > 0 || sensorTestCmd > 0) 
        {
            Serial.println("[Main] Modbus Command Received:");
            Serial.printf("       Speed=%d, Dispense=%d, Home=%d, Calib=%d, SensorTest=%d\n", 
                         speed, dispense, homeCmd, calibCmd, sensorTestCmd);
            Serial.printf("       System: Ready=%s, HomeFound=%s\n", 
                         systemReady ? "YES" : "NO", homePositionFound ? "YES" : "NO");
        }

        // Process sensor test commands first (for debugging)
        if (sensorTestCmd != SENSOR_TEST_NONE) {
            Serial.printf("[Main] Processing Sensor Test Command: %d\n", sensorTestCmd);
            processSensorTestCommand(sensorTestCmd);
            rtu.holdingRegisterWrite(ADDR_REG_SENSOR_TEST, SENSOR_TEST_NONE); // Clear command
        }

        // Process home commands (highest priority)
        if (homeCmd != HOME_CMD_NONE) {
            Serial.printf("[Main] 🏠 Processing Home Command: %d\n", homeCmd);
            processHomeCommand(homeCmd);
            rtu.holdingRegisterWrite(ADDR_REG_HOME, HOME_CMD_NONE); // Clear command
            Serial.println("[Main] Home command processed");
        }

        // Process calibration commands  
        if (calibCmd != CALIB_CMD_NONE) {
            processCalibrationCommand(calibCmd);
            rtu.holdingRegisterWrite(ADDR_REG_CALIBRATE, CALIB_CMD_NONE); // Clear command
        }

        // Process dispense command (only if system is ready after homing)
        if (speed > 0 && dispense > 0) {
            if (systemReady && homePositionFound) {  // Check both flags
                // Validate inputs
                speed = constrain(speed, MOTOR_PWM_MIN, MOTOR_PWM_MAX);
                dispense = constrain(dispense, 1, DISPENSE_MAX_ROTATIONS);
                
                Serial.printf("[Main] Dispense: %d rotations @ speed %d\n", dispense, speed);
                
                // Start dispensing
                dispense_start(dispense, speed);
                
                // Clear dispense command after execution
                rtu.holdingRegisterWrite(ADDR_REG_DISP, 0);
                rtu.holdingRegisterWrite(ADDR_REG_SPEED, 0);
                Serial.println("[Main] Dispensing started successfully");
                
            } else {
                // System not ready - clear invalid command
                if (!homePositionFound) {
                    Serial.println("[Main] ERROR: ต้องทำ Home Detection ก่อน!");
                    Serial.println("[Main] INFO: ใช้ Register 12 = 1 เพื่อเริ่ม Enhanced Home Detection");
                } else {
                    Serial.println("[Main] ERROR: ระบบไม่พร้อม!");
                }
                rtu.holdingRegisterWrite(ADDR_REG_DISP, 0); // Clear invalid command
                rtu.holdingRegisterWrite(ADDR_REG_SPEED, 0); // Clear speed as well
            }
        }
        
        // Update all status registers
        updateStatusRegisters();
    }
    
    delay(10);  // Small delay for system stability
}
