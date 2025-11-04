
#include "dispense.h"
#include "modbus_handler.h"  // สำหรับ isEnhancedHomingActive

// Global variables
volatile uint32_t rotationCounter = 0;  // Motor rotation counter (1 PPR)
uint32_t targetRotations = 0;           // Target rotation count
bool dispenseActive = false;            // Dispense operation active flag
bool homePositionFound = false;         // Home position status

// ===== PHASE 1: SENSOR HEALTH MONITORING =====
typedef struct {
    bool isConnected;           // เซ็นเซอร์เชื่อมต่ออยู่หรือไม่
    bool isWorking;             // เซ็นเซอร์ทำงานปกติหรือไม่
    unsigned long lastTriggerTime; // ครั้งสุดท้ายที่เซ็นเซอร์ทำงาน
    uint32_t triggerCount;      // จำนวนครั้งที่เซ็นเซอร์ทำงาน
    float averageInterval;      // ช่วงเวลาเฉลี่ยระหว่างการทำงาน
    unsigned long lastHealthCheck; // ครั้งสุดท้ายที่ตรวจสุขภาพ
} SensorHealth_t;

SensorHealth_t sensorHealth = {false, false, 0, 0, 0.0, 0};

void _dispenseISR() 
{
    // ===== SENSOR HEALTH MONITORING =====
    sensorHealth.lastTriggerTime = millis();
    sensorHealth.triggerCount++;
    sensorHealth.isWorking = true;  // เซ็นเซอร์ทำงาน
    
    // ถ้า Enhanced Homing กำลังทำงาน ไม่ต้อง increment rotation counter
    if (isEnhancedHomingActive) {
        Serial.println("[ISR] Enhanced Homing active - ไม่นับ rotation");
        return;
    }
    
    // Increment rotation counter on each sensor trigger (1 PPR) - เฉพาะตอน dispense
    rotationCounter++;
    
    // Check if target rotations reached
    if (rotationCounter >= targetRotations && dispenseActive) {
        dispense_stop();
        Serial.printf("[Dispense] Target reached: %d rotations\n", rotationCounter);
    }
}

void dispenseInit()
{   
    // Initialize sensor pin for motor rotation detection AND home position (using only SEN_1)
    pinMode(SEN_1_PIN, INPUT_PULLUP);  // Single sensor for both rotation and home detection

    attachInterrupt(digitalPinToInterrupt(SEN_1_PIN), _dispenseISR, FALLING);  // Single sensor interrupt
    
    // Initialize variables
    rotationCounter = 0;
    targetRotations = 0;
    dispenseActive = false;
    homePositionFound = false;
    
    // ===== INITIALIZE SENSOR HEALTH MONITORING =====
    sensorHealth.isConnected = true;  // สมมติว่าเชื่อมต่อแล้ว
    sensorHealth.isWorking = false;   // ยังไม่ทราบว่าทำงานหรือไม่
    sensorHealth.lastTriggerTime = millis();
    sensorHealth.triggerCount = 0;
    sensorHealth.averageInterval = 0.0;
    sensorHealth.lastHealthCheck = millis();
    
    // Check initial sensor state
    Serial.printf("[Dispense] SEN_1 (rotation & home): %s\n", digitalRead(SEN_1_PIN) ? "HIGH" : "LOW");
    
    Serial.println("[Dispense] Motor rotation control initialized (1 PPR - Single Sensor)");
    Serial.println("[Sensor Health] เริ่มตรวจสอบสุขภาพเซ็นเซอร์");
}

// ===== PHASE 1: SENSOR HEALTH MONITORING FUNCTIONS =====
void monitorSensorHealth() {
    unsigned long currentTime = millis();
    
    // ตรวจสอบทุกๆ 2 วินาที
    if (currentTime - sensorHealth.lastHealthCheck > 2000) {
        
        // ถ้ามอเตอร์กำลังหมุน แต่เซ็นเซอร์ไม่ทำงาน = มีปัญหา
        if (flag_motorRunning) {
            if (currentTime - sensorHealth.lastTriggerTime > 5000) {
                if (sensorHealth.isWorking) {  // แจ้งครั้งเดียวเมื่อเปลี่ยนสถานะ
                    sensorHealth.isWorking = false;
                    Serial.println("[Sensor Health] เตือน: เซ็นเซอร์ไม่ทำงานขณะมอเตอร์หมุน!");
                    Serial.printf("[Sensor Health] ไม่มีสัญญาณมา %lu วินาที\n", 
                                 (currentTime - sensorHealth.lastTriggerTime) / 1000);
                }
            } else {
                if (!sensorHealth.isWorking) {  // แจ้งเมื่อกลับมาทำงานปกติ
                    sensorHealth.isWorking = true;
                    Serial.println("[Sensor Health] เซ็นเซอร์ทำงานปกติแล้ว");
                }
            }
        }
        
        // คำนวณช่วงเวลาเฉลี่ยระหว่างการทำงานของเซ็นเซอร์
        if (sensorHealth.triggerCount > 1) {
            sensorHealth.averageInterval = 
                (float)(currentTime - sensorHealth.lastTriggerTime) / sensorHealth.triggerCount;
        }
        
        sensorHealth.lastHealthCheck = currentTime;
    }
}

void printSensorHealthStatus() {
    Serial.println("\n===== SENSOR HEALTH STATUS =====");
    Serial.printf("เชื่อมต่อ: %s\n", sensorHealth.isConnected ? "ปกติ" : "ขาดการเชื่อมต่อ");
    Serial.printf("ทำงาน: %s\n", sensorHealth.isWorking ? "ปกติ" : "ผิดปกติ");
    Serial.printf("จำนวนการทำงาน: %d ครั้ง\n", sensorHealth.triggerCount);
    Serial.printf("ครั้งสุดท้าย: %lu วินาทีที่แล้ว\n", 
                 (millis() - sensorHealth.lastTriggerTime) / 1000);
    if (sensorHealth.averageInterval > 0) {
        Serial.printf("ช่วงเวลาเฉลี่ย: %.1f วินาที\n", sensorHealth.averageInterval / 1000.0);
    }
    Serial.println("==================================\n");
}

// ===== ISR CONTROL FUNCTIONS =====
void disableRotationISR() {
    detachInterrupt(digitalPinToInterrupt(SEN_1_PIN));
    Serial.println("[ISR Control] 🚫 ปิด Rotation ISR");
}

void enableRotationISR() {
    attachInterrupt(digitalPinToInterrupt(SEN_1_PIN), _dispenseISR, FALLING);
    Serial.println("[ISR Control] เปิด Rotation ISR");
}

void dispense_start(uint32_t rotations, int motorSpeed)
{
    // Validate input parameters
    if (rotations == 0 || rotations > DISPENSE_MAX_ROTATIONS) {
        Serial.printf("[Dispense] ERROR: Invalid rotation count %d (max: %d)\n", 
                     rotations, DISPENSE_MAX_ROTATIONS);
        return;
    }
    
    // Check if home position is found
    if (!homePositionFound) {
        Serial.println("[Dispense] ERROR: Cannot dispense - Home position not found!");
        Serial.println("[Dispense] Please run homing sequence first");
        return;
    }
    
    // Stop any current operation
    dispense_stop();
    
    // Reset counter and set target
    rotationCounter = 0;
    targetRotations = rotations;
    dispenseActive = true;
    
    // Start motor
    startMotor(motorSpeed, true);  // Forward direction
    
    Serial.printf("[Dispense] Started: Target=%d rotations, Speed=%d\n", 
                 targetRotations, motorSpeed);
    Serial.printf("[Dispense] Current position will be tracked...\n");
}

void dispense_stop()
{
    // Stop motor
    stopMotor();
    
    // Update state
    dispenseActive = false;
    
    Serial.printf("[Dispense] Stopped at %d/%d rotations\n", 
                 rotationCounter, targetRotations);
}

void dispense_update()
{
    // ===== PHASE 1: SENSOR HEALTH MONITORING =====
    monitorSensorHealth();
    
    // Auto-stop safety check (in case interrupt doesn't trigger)
    if (dispenseActive && rotationCounter >= targetRotations) {
        dispense_stop();
    }
    
    // Additional safety: stop if motor should be running but flag says it's not
    if (dispenseActive && !flag_motorRunning) {
        Serial.println("[Dispense] WARNING: Motor stopped unexpectedly");
        dispenseActive = false;
    }
}

// Additional functions needed by modbus_handler
void resetCounter()
{
    rotationCounter = 0;
    Serial.println("[Dispense] Rotation counter reset");
}

void returnToHome()
{
    if (!homePositionFound) {
        Serial.println("[Dispense] ERROR: Home position not found. Cannot return home.");
        return;
    }
    
    Serial.println("[Dispense] Returning to home position...");
    // Reset counter since we're going back to home
    resetCounter();
    
    // Start motor in reverse direction to return to home
    startMotor(MOTOR_PWM_SLOW, false);  // Reverse direction at slow speed
    
    // Wait for sensor (SEN_1) to be triggered (same sensor for home detection)
    unsigned long startTime = millis();
    while (digitalRead(SEN_1_PIN) != LOW && (millis() - startTime) < 30000) {
        delay(100);  // Small delay to prevent tight loop
    }

    if (digitalRead(SEN_1_PIN) == LOW) {
        stopMotor(true);  // Stop with braking
        resetCounter();   // Reset counter at home position
        Serial.println("[Dispense] Successfully returned to home position");
    } else {
        stopMotor(true);
        Serial.println("[Dispense] ERROR: Failed to return to home position (timeout)");
    }
}

bool isAtTargetPosition(float target)
{
    // Convert rotation counter to position (assuming 1 rotation = 1.0 position unit)
    float currentPos = (float)rotationCounter;
    float tolerance = 0.1;  // Position tolerance
    
    return (abs(currentPos - target) <= tolerance);
}

float getCurrentPosition()
{
    // Return current position based on rotation counter
    return (float)rotationCounter;
}