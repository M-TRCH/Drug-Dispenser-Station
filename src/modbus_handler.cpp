#include "modbus_handler.h"

ModbusRTUServerClass rtu;

// ===== SHARED VARIABLES =====
bool isEnhancedHomingActive = false;  // แชร์สถานะ Enhanced Homing

// Internal state variables
static bool homingInProgress = false;
static bool calibrationInProgress = false;
static uint16_t currentErrorCode = ERR_NONE;
static uint32_t lastCommandTime = 0;

// ===== PHASE 1: ENHANCED HOME DETECTION =====
// Variables for enhanced home detection
static int consecutiveDetections = 0;
static unsigned long lastDetectionTime = 0;
static const int REQUIRED_DETECTIONS = 3;       // ต้องตรวจจับ 3 ครั้งติดต่อกัน
static const unsigned long DETECTION_INTERVAL = 50; // ห่างกัน 50ms
static bool enhancedHomingActive = false;

void modbusInit() 
{
    // Initialize Modbus RTU server
    if (!rtu.begin(rs485, MODBUS_SLAVE_ID, RS485_BAUDRATE, SERIAL_8N1)) {
        Serial.println("[Modbus] ERROR: Failed to start RTU Server!");
        return;
    }

    // Configure holding registers (เพิ่มขนาด register pool)
    rtu.configureHoldingRegisters(0, MODBUS_REGISTER_COUNT);

    // Initialize register values
    rtu.holdingRegisterWrite(ADDR_REG_SPEED, 0);                 // Initialize to 0
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

    Serial.printf("[Modbus] RTU Server started (Slave ID: %d, Baudrate: %d)\n", MODBUS_SLAVE_ID, RS485_BAUDRATE);
    Serial.println("\n=====================================");
    Serial.println("DRUG DISPENSER - MODBUS READY");
    Serial.println("=====================================");
    Serial.println("CONNECTION:");
    Serial.printf("   Slave ID: %d\n", MODBUS_SLAVE_ID);
    Serial.printf("   Baudrate: %d\n", RS485_BAUDRATE);
    Serial.printf("   Port: RS485 (Serial3)\n");
    Serial.println("\nREGISTER MAP:");
    Serial.printf("   Reg %d: Motor Speed (1000-4000)\n", ADDR_REG_SPEED);
    Serial.printf("   Reg %d: Dispense Rotations (1-99)\n", ADDR_REG_DISP);
    Serial.printf("   Reg %d: Home Command (1=find, 2=return)\n", ADDR_REG_HOME);
    Serial.printf("   Reg %d: Calibration (1=start)\n", ADDR_REG_CALIBRATE);
    Serial.printf("   Reg %d: Status (read-only)\n", ADDR_REG_STATUS);
    Serial.printf("   Reg %d: Position x100 (read-only)\n", ADDR_REG_POSITION);
    Serial.printf("   Reg %d: Error Code (read-only)\n", ADDR_REG_ERROR);
    Serial.println("\nQUICK TEST:");
    Serial.println("   1. Write 3000 to Reg 10");
    Serial.println("   2. Write 5 to Reg 11");
    Serial.println("   3. Monitor Reg 20-22 for status");
    Serial.println("=====================================\n");
}

// Remove modbusHandler() function since we're using the main.cpp approach from the commit

void processHomeCommand(int homeCmd)
{
    switch (homeCmd) {
        case HOME_CMD_FIND:
            if (!homingInProgress && !dispenseActive) {
                Serial.println("[Enhanced Home] เริ่มค้นหาตำแหน่ง Home (Enhanced Detection)");
                Serial.printf("[Enhanced Home] Initial State - Sensor: %s, Motor: %s\n",
                             digitalRead(SEN_1_PIN) == LOW ? "LOW" : "HIGH",
                             flag_motorRunning ? "RUNNING" : "STOPPED");
                
                homingInProgress = true;
                enhancedHomingActive = true;  // เปิดใช้ Enhanced Detection
                isEnhancedHomingActive = true; // แชร์สถานะกับไฟล์อื่น
                currentErrorCode = ERR_NONE;
                lastCommandTime = millis();
                
                // Reset Enhanced Detection variables
                consecutiveDetections = 0;
                lastDetectionTime = 0;
                
                // ปิด Rotation ISR ขณะ Enhanced Homing
                disableRotationISR();
                
                // Start homing process with single sensor
                resetCounter();
                startMotor(MOTOR_PWM_SLOW, true);  // Use slow speed for homing
                
                Serial.printf("[Enhanced Home] ต้องตรวจจับต่อเนื่อง %d ครั้ง เพื่อยืนยัน\n", REQUIRED_DETECTIONS);
                Serial.printf("[Enhanced Home] เริ่มหมุนมอเตอร์ที่ความเร็ว %d...\n", MOTOR_PWM_SLOW);
                Serial.println("[Enhanced Home] เริ่มติดตามสถานะเซ็นเซอร์...");
                
            } else {
                currentErrorCode = ERR_INVALID_COMMAND;
                Serial.printf("[Enhanced Home] ERROR: ไม่สามารถเริ่ม homing ได้ - HomingProgress: %s, DispenseActive: %s\n",
                             homingInProgress ? "true" : "false",
                             dispenseActive ? "true" : "false");
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

// ===== PHASE 1: ENHANCED HOME DETECTION FUNCTIONS =====
void enhancedHomeDetection() {
    if (!enhancedHomingActive) return;
    
    static bool sensorCurrentlyLow = false;
    static unsigned long continuousLowStartTime = 0;
    static unsigned long lastDebugTime = 0;
    static const unsigned long MIN_CONTINUOUS_TIME = 100; // ต้อง LOW ต่อเนื่อง 100ms
    
    bool sensorState = (digitalRead(SEN_1_PIN) == LOW);
    
    // Debug sensor state ทุก 500ms
    if (millis() - lastDebugTime > 500) {
        Serial.printf("[Enhanced Home] 🔍 Sensor: %s | Detections: %d/%d | Motor: %s\n", 
                     sensorState ? "LOW" : "HIGH", 
                     consecutiveDetections, REQUIRED_DETECTIONS,
                     flag_motorRunning ? "RUN" : "STOP");
        lastDebugTime = millis();
    }
    
    if (sensorState && !sensorCurrentlyLow) {
        // เริ่มต้น LOW period
        sensorCurrentlyLow = true;
        continuousLowStartTime = millis();
        Serial.println("[Enhanced Home] � เซ็นเซอร์เริ่ม LOW - เริ่มนับเวลา");
        
    } else if (!sensorState && sensorCurrentlyLow) {
        // จบ LOW period - ตรวจสอบว่า LOW นานพอหรือไม่
        sensorCurrentlyLow = false;
        unsigned long lowDuration = millis() - continuousLowStartTime;
        
        Serial.printf("[Enhanced Home] ⬜ เซ็นเซอร์กลับ HIGH - LOW นาน %lu ms\n", lowDuration);
        
        if (lowDuration >= MIN_CONTINUOUS_TIME) {
            consecutiveDetections++;
            Serial.printf("[Enhanced Home] ตรวจจับที่ %d/%d (LOW นาน %lu ms)\n", 
                         consecutiveDetections, REQUIRED_DETECTIONS, lowDuration);
            
            if (consecutiveDetections >= REQUIRED_DETECTIONS) {
                Serial.println("[Enhanced Home] ครบจำนวนที่ต้องการแล้ว - ยืนยัน Home!");
                confirmHomePosition();
                consecutiveDetections = 0;
                enhancedHomingActive = false;
                return;
            }
        } else {
            Serial.printf("[Enhanced Home] LOW สั้นเกินไป: %lu ms (ต้อง >= %lu ms)\n", 
                         lowDuration, MIN_CONTINUOUS_TIME);
        }
        
    } else if (sensorCurrentlyLow) {
        // ยังคง LOW อยู่ - แสดง progress
        unsigned long currentLowTime = millis() - continuousLowStartTime;
        if (currentLowTime % 500 == 0) { // แสดงทุก 500ms
            Serial.printf("[Enhanced Home] LOW ต่อเนื่อง: %lu ms\n", currentLowTime);
        }
    }
}

void confirmHomePosition() {
    Serial.println("[Enhanced Home] ยืนยัน Home Position แล้ว!");
    
    // Stop motor
    stopMotor(false);
    delay(200);  // Allow motor to settle
    
    // Set home position
    resetCounter();
    homePositionFound = true;
    homingInProgress = false;
    enhancedHomingActive = false;
    isEnhancedHomingActive = false; // แชร์สถานะกับไฟล์อื่น
    currentErrorCode = ERR_NONE;
    
    // เปิด Rotation ISR กลับมา
    enableRotationISR();
    
    Serial.println("[Enhanced Home] ตั้งค่า Home Position เรียบร้อย!");
    Serial.printf("[Enhanced Home] ตำแหน่งปัจจุบัน: %d\n", rotationCounter);
}

// ===== HOME DETECTION LOGIC =====
void updateHomingProcess()
{
    if (!homingInProgress) return;
    
    // ใช้ Enhanced Home Detection แทนการตรวจจับแบบเดิม
    enhancedHomeDetection();
    
    // Check timeout
    if (millis() - lastCommandTime > 30000) {  // 30 second timeout
        Serial.println("[Enhanced Home] ERROR: Timeout! หาตำแหน่ง Home ไม่สำเร็จ");
        stopMotor(false);
        homingInProgress = false;
        enhancedHomingActive = false;
        isEnhancedHomingActive = false; // แชร์สถานะกับไฟล์อื่น
        consecutiveDetections = 0;
        currentErrorCode = ERR_HOME_NOT_FOUND;
        
        // เปิด Rotation ISR กลับมา
        enableRotationISR();
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

// ===== SENSOR TEST COMMAND PROCESSING =====
void processSensorTestCommand(int testCmd)
{
    switch (testCmd) {
        case SENSOR_TEST_START:
            if (!homingInProgress && !dispenseActive && !calibrationInProgress) {
                Serial.println("[Sensor Test] เริ่มทดสอบเซ็นเซอร์ 10 ครั้ง...");
                
                for (int i = 0; i < 10; i++) {
                    bool sensorState = (digitalRead(SEN_1_PIN) == LOW);
                    Serial.printf("[Sensor Test] ครั้งที่ %d: SEN_1 = %s (Raw: %d)\n", 
                                 i+1, 
                                 sensorState ? "LOW (ตรวจจับ)" : "HIGH (ไม่ตรวจจับ)",
                                 digitalRead(SEN_1_PIN));
                    delay(200);
                }
                
                Serial.println("[Sensor Test] ทดสอบเซ็นเซอร์เสร็จสิ้น");
                currentErrorCode = ERR_NONE;
                
            } else {
                currentErrorCode = ERR_INVALID_COMMAND;
                Serial.println("[Sensor Test] ERROR: ไม่สามารถทดสอบเซ็นเซอร์ได้ - ระบบไม่ว่าง");
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
    
    // Check if system is ready (add systemReady check)
    extern bool systemReady;
    if (systemReady && homePositionFound) {
        // System is ready for operation
    } else if (!homePositionFound) {
        currentErrorCode = ERR_HOME_NOT_FOUND;
        status |= STATUS_ERROR;
    }
    
    rtu.holdingRegisterWrite(ADDR_REG_STATUS, status);
    
    // Update position register (scaled by 100 for precision)
    int16_t position = (int16_t)(getCurrentPosition() * 100);
    rtu.holdingRegisterWrite(ADDR_REG_POSITION, position);
    
    // Update error register
    rtu.holdingRegisterWrite(ADDR_REG_ERROR, currentErrorCode);
}
