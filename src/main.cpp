#include <Arduino.h>
#include <motor_control.h>
#include <system.h>

// ================= SYSTEM VARIABLES =================
extern unsigned long targetCount;
bool systemActive = false;
unsigned long stopTime = 0;

// ================= SETUP =================
void setup() {
  targetCount = 10;
  Serial3.begin(115200);
  analogReadResolution(ANALOG_READ_RESOLUTION);

  // Sensor Input
  pinMode(SEN_1_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(SEN_1_PIN), sensorTriggerISR, FALLING);  // สำหรับ NPN sensor

  // Motor PWM Pins
  pinMode(PWM_A_PIN, OUTPUT);
  pinMode(PWM_B_PIN, OUTPUT);
  pinMode(PWM_C_PIN, OUTPUT);

  // Control Buttons
  pinMode(SW_START_PIN, INPUT_PULLUP);
  pinMode(SW_CALC_PIN, INPUT_PULLUP);

  // LED Status
  pinMode(LED_STATUS_PIN, OUTPUT);

  // PWM config
  analogWriteFrequency(PWM_FREQUENCY);
  analogWriteResolution(PWM_RESOLUTION);
  analogWrite(PWM_A_PIN, 0);
  analogWrite(PWM_B_PIN, 0);
  analogWrite(PWM_C_PIN, 0);

  digitalWrite(LED_STATUS_PIN, LOW);

  Serial3.println("✅ System Ready...");
  Serial3.printf("Target Count = %d\n", targetCount);
}

// ================= MAIN LOOP =================
void loop() {
  // ----- Start / Stop toggle -----
  if (digitalRead(SW_START_PIN) == LOW) {
    delay(150); // debounce
    if (!systemActive) {
      systemActive = true;
      motorRunning = true;
      objectCount = 0;
      stopTime = 0;
      Serial3.println("▶️ Motor started");
    } else {
      systemActive = false;
      motorRunning = false;
      analogWrite(PWM_A_PIN, 0);
      analogWrite(PWM_B_PIN, 0);
      analogWrite(PWM_C_PIN, 0);
      digitalWrite(LED_STATUS_PIN, LOW);
      Serial3.println("⏹ System stopped manually");
    }
    while (digitalRead(SW_START_PIN) == LOW); // รอปล่อยปุ่ม
  }

  // ----- Main Motor Control -----
  if (systemActive && motorRunning) {
    analogWrite(PWM_A_PIN, 3500); // speed
    analogWrite(PWM_B_PIN, 0);
    analogWrite(PWM_C_PIN, 0);
    digitalWrite(LED_STATUS_PIN, HIGH);

    if (objectCount >= targetCount) {
      motorRunning = false;
      analogWrite(PWM_A_PIN, 0);
      analogWrite(PWM_B_PIN, 0);
      analogWrite(PWM_C_PIN, 0);
      digitalWrite(LED_STATUS_PIN, LOW);
      stopTime = millis();
      Serial3.println("✅ Target reached → Stop for 5s");
    }
  }

  // ----- Restart after delay -----
  if (systemActive && !motorRunning && stopTime > 0) {
    if (millis() - stopTime >= 5000) {
      objectCount = 0;
      motorRunning = true;
      stopTime = 0;
      Serial3.println("🔄 Restart after delay");
    }
  }

  // ----- Optional: Adjust target count with CALC -----
  if (digitalRead(SW_CALC_PIN) == LOW) {
    delay(200);
    targetCount++;
    if (targetCount > 50) targetCount = 1;
    Serial3.printf("🎯 Target Count Set to: %d\n", targetCount);
    while (digitalRead(SW_CALC_PIN) == LOW);
  }
}
