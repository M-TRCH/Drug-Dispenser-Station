/**
 * @file Enhanced Home Sensor Analysis & Improvements
 * @brief Proposed improvements for sensorHome functionality
 * @date November 4, 2025
 */

// ===== CURRENT ISSUES & PROPOSED SOLUTIONS =====

/* 
PROBLEM 1: Single Point Home Detection
- ปัจจุบัน: ใช้ trigger เดียวสำหรับ home position
- ปัญหา: อาจไม่แม่นยำ, noise sensitivity
- แก้ไข: Implement Multi-Stage Home Detection
*/

// SOLUTION 1: Enhanced Home Detection with Validation
void enhancedHomeDetection() {
    static int consecutiveDetections = 0;
    static unsigned long lastDetectionTime = 0;
    const int REQUIRED_DETECTIONS = 3;  // ต้อง detect ติดต่อกัน 3 ครั้ง
    const unsigned long DETECTION_INTERVAL = 50; // ห่างกัน 50ms
    
    if (digitalRead(SEN_1_PIN) == LOW) {
        unsigned long currentTime = millis();
        
        if (currentTime - lastDetectionTime > DETECTION_INTERVAL) {
            consecutiveDetections++;
            lastDetectionTime = currentTime;
            
            if (consecutiveDetections >= REQUIRED_DETECTIONS) {
                // Confirmed home position
                confirmHomePosition();
                consecutiveDetections = 0;
            }
        }
    } else {
        consecutiveDetections = 0; // Reset if sensor goes HIGH
    }
}

/*
PROBLEM 2: No Sensor State Monitoring
- ปัจจุบัน: ไม่มีการตรวจสอบสถานะเซ็นเซอร์
- ปัญหา: ไม่รู้ว่าเซ็นเซอร์ทำงานปกติหรือไม่
- แก้ไข: Add Sensor Health Monitoring
*/

// SOLUTION 2: Sensor Health Monitoring
typedef struct {
    bool isConnected;
    bool isWorking;
    unsigned long lastTriggerTime;
    uint32_t triggerCount;
    float averageInterval;
} SensorHealth_t;

SensorHealth_t sensorHealth = {false, false, 0, 0, 0.0};

void monitorSensorHealth() {
    static unsigned long lastCheckTime = 0;
    static unsigned long lastTriggerTime = 0;
    
    if (millis() - lastCheckTime > 1000) { // Check every second
        // Check if sensor has been triggered recently during movement
        if (flag_motorRunning) {
            if (millis() - sensorHealth.lastTriggerTime > 5000) {
                sensorHealth.isWorking = false;
                Serial.println("[Sensor] WARNING: No sensor activity during movement!");
            } else {
                sensorHealth.isWorking = true;
            }
        }
        
        // Calculate average interval between triggers
        if (sensorHealth.triggerCount > 1) {
            sensorHealth.averageInterval = 
                (float)(millis() - lastTriggerTime) / sensorHealth.triggerCount;
        }
        
        lastCheckTime = millis();
    }
}

/*
PROBLEM 3: Simple Timeout Only
- ปัจจุบัน: ใช้ timeout 30 วินาทีเพียงอย่างเดียว
- ปัญหา: อาจ timeout เร็วเกินไปหรือช้าเกินไป
- แก้ไข: Adaptive Timeout with Movement Detection
*/

// SOLUTION 3: Adaptive Homing with Movement Detection
typedef enum {
    HOMING_INIT,
    HOMING_MOVING,
    HOMING_SEARCHING,
    HOMING_VALIDATING,
    HOMING_COMPLETE,
    HOMING_FAILED
} HomingState_t;

HomingState_t homingState = HOMING_INIT;

void adaptiveHomingProcess() {
    static unsigned long stateStartTime = 0;
    static uint32_t lastRotationCount = 0;
    static int validationCount = 0;
    
    switch (homingState) {
        case HOMING_INIT:
            Serial.println("[Enhanced Homing] Starting adaptive homing...");
            resetCounter();
            startMotor(MOTOR_PWM_SLOW, true);
            homingState = HOMING_MOVING;
            stateStartTime = millis();
            break;
            
        case HOMING_MOVING:
            // Check if motor is actually moving
            if (rotationCounter > lastRotationCount) {
                lastRotationCount = rotationCounter;
                homingState = HOMING_SEARCHING;
            } else if (millis() - stateStartTime > 5000) {
                // Motor not moving after 5 seconds
                homingState = HOMING_FAILED;
            }
            break;
            
        case HOMING_SEARCHING:
            if (digitalRead(SEN_1_PIN) == LOW) {
                Serial.println("[Enhanced Homing] Home sensor triggered - validating...");
                stopMotor(true);
                delay(100);
                homingState = HOMING_VALIDATING;
                stateStartTime = millis();
                validationCount = 0;
            } else if (millis() - stateStartTime > 30000) {
                homingState = HOMING_FAILED;
            }
            break;
            
        case HOMING_VALIDATING:
            // Multiple validation checks
            if (digitalRead(SEN_1_PIN) == LOW) {
                validationCount++;
            } else {
                validationCount = 0; // Reset if sensor goes HIGH
            }
            
            if (validationCount >= 5) { // 5 consecutive LOW readings
                homingState = HOMING_COMPLETE;
            } else if (millis() - stateStartTime > 2000) {
                // Validation timeout - restart search
                homingState = HOMING_SEARCHING;
                startMotor(MOTOR_PWM_SLOW, true);
            }
            break;
            
        case HOMING_COMPLETE:
            resetCounter();
            homePositionFound = true;
            systemReady = true;
            Serial.println("[Enhanced Homing] ✅ Home position confirmed!");
            break;
            
        case HOMING_FAILED:
            stopMotor(true);
            Serial.println("[Enhanced Homing] ❌ Homing failed!");
            break;
    }
}

/*
PROBLEM 4: No Return-to-Home Optimization
- ปัจจุบัน: การกลับ home ใช้วิธีเดียวกับการหา home
- ปัญหา: ไม่มีการ optimize สำหรับการกลับ home
- แก้ไข: Smart Return-to-Home
*/

// SOLUTION 4: Smart Return-to-Home
void smartReturnToHome() {
    if (!homePositionFound) {
        Serial.println("[Smart Return] Home position not set. Running full homing...");
        adaptiveHomingProcess();
        return;
    }
    
    uint32_t currentPosition = rotationCounter;
    Serial.printf("[Smart Return] Current: %d, returning to home (0)...\n", currentPosition);
    
    if (currentPosition == 0) {
        Serial.println("[Smart Return] Already at home position!");
        return;
    }
    
    // Calculate direction and distance
    bool reverseDirection = (currentPosition > 0);
    uint32_t approximateDistance = currentPosition;
    
    // Start movement with appropriate speed
    int speed = (approximateDistance < 10) ? MOTOR_PWM_SLOW : MOTOR_PWM_DEFAULT;
    startMotor(speed, !reverseDirection); // Reverse direction
    
    // Monitor for home sensor while counting down
    unsigned long startTime = millis();
    while (millis() - startTime < 15000) { // Shorter timeout for return
        if (digitalRead(SEN_1_PIN) == LOW) {
            stopMotor(true);
            delay(100);
            
            // Validate home position
            if (digitalRead(SEN_1_PIN) == LOW) {
                resetCounter();
                Serial.println("[Smart Return] ✅ Successfully returned to home!");
                return;
            }
        }
        delay(10);
    }
    
    // Timeout - perform full homing
    Serial.println("[Smart Return] Timeout - performing full homing...");
    homePositionFound = false; // Reset flag
    adaptiveHomingProcess();
}

/*
PROBLEM 5: No Performance Metrics
- ปัจจุบัน: ไม่มีการเก็บข้อมูลประสิทธิภาพ
- ปัญหา: ไม่รู้ว่าระบบทำงานได้ดีแค่ไหน
- แก้ไข: Add Performance Tracking
*/

// SOLUTION 5: Performance Metrics
typedef struct {
    uint32_t totalHomingAttempts;
    uint32_t successfulHomings;
    uint32_t failedHomings;
    float averageHomingTime;
    uint32_t totalReturnToHomeAttempts;
    uint32_t successfulReturns;
    float homingAccuracy; // Percentage
} HomingMetrics_t;

HomingMetrics_t homingMetrics = {0, 0, 0, 0.0, 0, 0, 0.0};

void updateHomingMetrics(bool success, unsigned long duration) {
    homingMetrics.totalHomingAttempts++;
    
    if (success) {
        homingMetrics.successfulHomings++;
        // Update average time
        homingMetrics.averageHomingTime = 
            (homingMetrics.averageHomingTime * (homingMetrics.successfulHomings - 1) + duration) 
            / homingMetrics.successfulHomings;
    } else {
        homingMetrics.failedHomings++;
    }
    
    homingMetrics.homingAccuracy = 
        (float)homingMetrics.successfulHomings / homingMetrics.totalHomingAttempts * 100.0;
}

void printHomingStats() {
    Serial.println("\n===== HOMING PERFORMANCE STATS =====");
    Serial.printf("Total Attempts: %d\n", homingMetrics.totalHomingAttempts);
    Serial.printf("Success Rate: %.1f%%\n", homingMetrics.homingAccuracy);
    Serial.printf("Average Time: %.1f ms\n", homingMetrics.averageHomingTime);
    Serial.printf("Failed Attempts: %d\n", homingMetrics.failedHomings);
    Serial.println("====================================\n");
}

// ===== INTEGRATION EXAMPLE =====
void improvedHomingSystem() {
    // 1. Monitor sensor health continuously
    monitorSensorHealth();
    
    // 2. Use adaptive homing process
    adaptiveHomingProcess();
    
    // 3. Track performance
    // updateHomingMetrics(success, duration); // Called after each attempt
    
    // 4. Provide smart return-to-home
    // smartReturnToHome(); // When needed
}