#include <Arduino.h>
#include "system.h"
#include "motor_control.h"
#include "sensor_handler.h"
#include "modbus_handler.h"

// ================== STATE DEFINITIONS ==================
enum SystemState {
  STATE_IDLE,
  STATE_HOME,
  STATE_RUN,
  STATE_PAUSE
};

// ================== FUNCTION DECLARATIONS ==================
void handleButtons();
void handleHoming();
void handleRunning();
void handlePause();

// ================== VARIABLES ==================
SystemState systemState = STATE_IDLE;
unsigned long stopTime = 0;
unsigned long lastPrint = 0;

// ================== SETUP ==================
void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n[System] Dispenser Control with Homing");

  systemInit();
  motorControlInit();
  sensorInit();
  setupModbus();

  pinMode(SW_START_PIN, INPUT_PULLUP);
  pinMode(SW_CALC_PIN, INPUT_PULLUP);
  pinMode(LED_STATUS_PIN, OUTPUT);
  digitalWrite(LED_STATUS_PIN, LOW);

  motorVelocity = 2500;
  dispenseTarget = 10;
}

// ================== LOOP ==================
void loop() {
  handleButtons();

  switch (systemState) {
    case STATE_IDLE:
      // รอคำสั่งเริ่ม
      break;

    case STATE_HOME:
      handleHoming();
      break;

    case STATE_RUN:
      handleRunning();
      break;

    case STATE_PAUSE:
      handlePause();
      break;
  }

  handleSensorLogic();
  handleModbus(); // เพิ่ม Modbus handling

  // Debug status ทุก 1 วินาที
  if (millis() - lastPrint > 1000) {
    lastPrint = millis();
    Serial.printf("[Status] State:%d Count:%lu/%lu\n", systemState, objectCount, dispenseTarget);
  }
}

// ================== BUTTON HANDLER ==================
void handleButtons() {
  if (digitalRead(SW_START_PIN) == LOW) {
    delay(150);
    while (digitalRead(SW_START_PIN) == LOW);
    if (systemState == STATE_IDLE) {
      Serial.println("[User] START pressed -> HOMING...");
      resetCounter();
      startMotor(MOTOR_HOMING_SPEED, true);   // ใช้ความเร็วคงที่สำหรับ homing
      systemState = STATE_HOME;
    } else {
      Serial.println("[User] STOP pressed -> IDLE");
      stopMotor(false);
      systemState = STATE_IDLE;
      digitalWrite(LED_STATUS_PIN, LOW);
    }
  }

  if (digitalRead(SW_CALC_PIN) == LOW) {
    delay(200);
    while (digitalRead(SW_CALC_PIN) == LOW);
    dispenseTarget++;
    if (dispenseTarget > 50) dispenseTarget = 1;
    Serial.printf("[Config] Target set to %lu\n", dispenseTarget);
  }
}

// ================== BUTTON HANDLING (REMOVED DUPLICATE) ==================

// ================== HOMING FUNCTION ==================
void handleHoming() {
  if (digitalRead(SEN_1_PIN) == LOW) {
    delay(50); // debounce
    stopMotor(false);
    delay(200);
    resetCounter();               
    Serial.println("[Home] Position found, starting run...");
    digitalWrite(LED_STATUS_PIN, HIGH);
    startMotor(MOTOR_HOMING_SPEED, true);  // ใช้ความเร็วเดียวกับ homing
    systemState = STATE_RUN;
  }
}

// ================== RUNNING FUNCTION ==================
void handleRunning() {
  if (objectCount >= dispenseTarget) {
    stopMotor(false);
    digitalWrite(LED_STATUS_PIN, LOW);
    Serial.printf("[Cycle] Completed %lu items\n", objectCount);
    systemState = STATE_PAUSE;
    stopTime = millis();
  }
}

// ================== PAUSE FUNCTION ==================
void handlePause() {
  if (millis() - stopTime >= 5000) {
    Serial.println("[Cycle] Restarting HOMING...");
    startMotor(MOTOR_HOMING_SPEED, true);  // ใช้ความเร็วคงที่เดียวกัน
    systemState = STATE_HOME;
  }
}
