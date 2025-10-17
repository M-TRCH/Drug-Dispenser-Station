#include "motor_control.h"
#include "system.h"
#include "sensor_handler.h"

bool motorRunning = false;
bool countingMode = false;
volatile unsigned long objectCount = 0;
unsigned long targetCount = DISPENSE_TARGET_DEFAULT;
unsigned long lastSensorTrigger = 0;
unsigned long lastSensorTriggerMicros = 0;

// === Modbus Variables ===
// Holding Registers (16-bit values)
unsigned int motorVelocity = MOTOR_VELOCITY_DEFAULT;  // ความเร็วมอเตอร์
unsigned int dispenseTarget = DISPENSE_TARGET_DEFAULT; // จำนวนเป้าหมาย  
unsigned int currentCount = 0;                        // จำนวนปัจจุบัน
unsigned int systemStatus = 0;                        // สถานะระบบ (0=IDLE, 1=HOMING, 2=READY, 3=RUNNING, 4=COMPLETED, 5=ERROR)

// Coils (1-bit values)
bool dispenserActivate = false;                       // เปิด/ปิด การจ่าย
bool systemStart = false;                             // เริ่มการทำงาน
bool systemStop = false;                              // หยุดการทำงาน
bool systemHome = false;                              // เริ่ม homing

void motorControlInit() {
  pinMode(PWM_A_PIN, OUTPUT);
  pinMode(PWM_B_PIN, OUTPUT);
  pinMode(DISPENSER_PIN, OUTPUT);

  dispenserOff();
  stopMotor(false);

  // ตั้งค่าความละเอียด ADC/PWM และความถี่ PWM
  analogReadResolution(ANALOG_READ_RESOLUTION);
  analogWriteResolution(MOTOR_PWM_RESOLUTION);
  analogWriteFrequency(MOTOR_PWM_FREQUENCY);

  // Serial3.printf("[MotorControl] Init complete | PWM: %uHz @ %u-bit\n",
  // MOTOR_PWM_FREQUENCY, MOTOR_PWM_RESOLUTION);
}

// ===== MOTOR CONTROL =====
void startMotor(int pwmSpeed, bool forward) {
  pwmSpeed = constrain(pwmSpeed, MOTOR_PWM_MIN, MOTOR_PWM_MAX);
  motorRunning = true;
  countingMode = true;

  Serial.print("[Motor] Starting motor - PWM Speed: ");
  Serial.print(pwmSpeed);
  Serial.print(", Direction: ");
  Serial.println(forward ? "Forward" : "Reverse");

  if (forward) {
    analogWrite(PWM_A_PIN, pwmSpeed);
    analogWrite(PWM_B_PIN, 0);
    Serial.print("[Motor] PWM_A_PIN (");
    Serial.print(PWM_A_PIN);
    Serial.print(") = ");
    Serial.println(pwmSpeed);
  } else {
    analogWrite(PWM_A_PIN, 0);
    analogWrite(PWM_B_PIN, pwmSpeed);
    Serial.print("[Motor] PWM_B_PIN (");
    Serial.print(PWM_B_PIN);
    Serial.print(") = ");
    Serial.println(pwmSpeed);
  }

  dispenserOn();
  delay(MOTOR_START_DELAY_MS);

  Serial.printf("[Motor] Started | Speed: %d | Dir: %s\n",
                 pwmSpeed, forward ? "Forward" : "Reverse");
}

void stopMotor(bool error) {
  Serial.println("[Motor] Stopping motor...");
  analogWrite(PWM_A_PIN, 0);
  analogWrite(PWM_B_PIN, 0);
  dispenserOff();
  motorRunning = false;
  countingMode = false;
  delay(MOTOR_STOP_DELAY_MS);

  Serial.printf("[Motor] Stopped | Error: %s\n", error ? "YES" : "NO");
}

void setMotorSpeed(int pwmSpeed) {
  pwmSpeed = constrain(pwmSpeed, MOTOR_PWM_MIN, MOTOR_PWM_MAX);
  analogWrite(PWM_A_PIN, pwmSpeed);
  // Serial3.printf("[Motor] Speed updated: %d\n", pwmSpeed);
}

// ===== DISPENSER =====
void dispenserOn() {
  digitalWrite(DISPENSER_PIN, DISPENSER_ACTIVE_LEVEL);
  Serial.print("[Dispenser] ON - Pin ");
  Serial.print(DISPENSER_PIN);
  Serial.print(" set to ");
  Serial.println(DISPENSER_ACTIVE_LEVEL ? "HIGH" : "LOW");
}

void dispenserOff() {
  digitalWrite(DISPENSER_PIN, !DISPENSER_ACTIVE_LEVEL);
  Serial.print("[Dispenser] OFF - Pin ");
  Serial.print(DISPENSER_PIN);
  Serial.print(" set to ");
  Serial.println(!DISPENSER_ACTIVE_LEVEL ? "HIGH" : "LOW");
}

// ===== COUNTER LOGIC =====
void resetCounter() {
  objectCount = 0;
  currentCount = 0; // อัปเดต Modbus variable
  // Serial.println("[Counter] Reset.");
}

bool isTargetReached() {
  return (objectCount >= dispenseTarget); // ใช้ dispenseTarget แทน targetCount
}

// ===== MODBUS VARIABLE SYNC =====
void syncModbusVariables() {
  // Sync holding registers
  targetCount = dispenseTarget;
  currentCount = objectCount;
  
  // Update system status based on current state
  // (systemStatus will be updated in state machine)
}

// ===== BUTTON HANDLING =====
bool lastStartButtonState = HIGH;
bool lastHomeButtonState = HIGH;
unsigned long lastButtonCheck = 0;

void handleSystemButtons() {
  if (millis() - lastButtonCheck < BUTTON_CHECK_INTERVAL_MS) return;
  lastButtonCheck = millis();
  
  // Start Button (PA1) 
  bool startButtonState = digitalRead(SW_START_PIN);
  if (lastStartButtonState == HIGH && startButtonState == LOW) {
    // Serial.println("START button pressed");
    systemStart = true; // Set Modbus coil
    delay(BUTTON_DEBOUNCE_TIME_MS); // Hardware debounce
  }
  lastStartButtonState = startButtonState;
  
  // Home Button (PA0)
  bool homeButtonState = digitalRead(SW_CALC_PIN);
  if (lastHomeButtonState == HIGH && homeButtonState == LOW) {
    // Serial.println("HOME button pressed");
    systemHome = true; // Set Modbus coil
    delay(BUTTON_DEBOUNCE_TIME_MS); // Hardware debounce
  }
  lastHomeButtonState = homeButtonState;
  
  // Process Modbus coils
  if (systemStart) {
    startOperation();
    systemStart = false; // Clear coil
  }
  
  if (systemStop) {
    stopMotor(false);
    systemStop = false; // Clear coil
  }
  
  if (systemHome) {
    // Handle homing (implement later)
    systemHome = false; // Clear coil
  }
}

// ===== STATE MANAGEMENT =====
unsigned long lastStatusUpdate = 0;

void updateSystemState() {
  // Sync Modbus variables
  syncModbusVariables();
  
  // Update status LED and monitoring
  if (millis() - lastStatusUpdate > 100) { // Update every 100ms
    lastStatusUpdate = millis();
    
    // Simple LED status
    if (motorRunning) {
      digitalWrite(LED_STATUS_PIN, millis() % 200 < 100); // Fast blink
    } else {
      digitalWrite(LED_STATUS_PIN, millis() % 1000 < 500); // Slow blink
    }
    
    // Check if target reached
    if (motorRunning && isTargetReached()) {
      stopMotor(false);
      // Serial.println("Target reached!");
      // Serial.print("Final count: ");
      // Serial.print(objectCount);
      // Serial.print(" / ");
      // Serial.println(dispenseTarget);
    }
  }
}

// ===== OPERATION CONTROL =====
void startOperation() {
  if (!motorRunning) {
    // Serial.println("▶Starting dispense operation...");
    resetCounter();
    startMotor(motorVelocity, true); // ใช้ motorVelocity จาก Modbus
    dispenserActivate = true; // Set Modbus coil
  } else {
    // Serial.println("Already running!");
  }
}

void printSystemStatus() {
  Serial.println("\n SYSTEM STATUS:");
  Serial.print("   Motor Speed: ");
  Serial.println(motorVelocity);
  Serial.print("   Target: ");
  Serial.println(dispenseTarget);
  Serial.print("   Count: ");
  Serial.print(objectCount);
  Serial.print(" / ");
  Serial.println(dispenseTarget);
  Serial.print("   Progress: ");
  Serial.print((float)objectCount / dispenseTarget * 100, 1);
  Serial.println("%");
  Serial.print("   Motor: ");
  Serial.println(motorRunning ? "RUNNING" : "STOPPED");
  Serial.print("   Dispenser: ");
  Serial.println(dispenserActivate ? "ACTIVE" : "INACTIVE");
  Serial.println("-------------------");
}

// ===== SENSOR INTERRUPT (จาก FS-N11N / FU-35FA) =====
// Function นี้ถูกย้ายไปอยู่ใน sensor_handler.cpp แล้ว
// void sensorTriggerISR() ใช้ sensorISR() แทน
