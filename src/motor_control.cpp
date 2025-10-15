#include "motor_control.h"
#include "system.h"

bool motorRunning = false;
bool countingMode = false;
volatile unsigned long objectCount = 0;
unsigned long targetCount = DISPENSE_TARGET_DEFAULT;
unsigned long lastSensorTrigger = 0;

void motorControlInit() {
  pinMode(PWM_A_PIN, OUTPUT);
  pinMode(PWM_B_PIN, OUTPUT);
  pinMode(DISPENSER_PIN, OUTPUT);

  dispenserOff();
  stopMotor(false);

  // ตั้งค่าความละเอียด ADC/PWM และความถี่ PWM
  analogReadResolution(ANALOG_READ_RESOLUTION);
  analogWriteResolution(PWM_RESOLUTION);
  analogWriteFrequency(PWM_FREQUENCY);

  Serial3.printf("[MotorControl] Init complete | PWM: %uHz @ %u-bit\n",
                 PWM_FREQUENCY, PWM_RESOLUTION);
}

// ===== MOTOR CONTROL =====
void startMotor(int pwmSpeed, bool forward) {
  pwmSpeed = constrain(pwmSpeed, MOTOR_PWM_MIN, MOTOR_PWM_MAX);
  motorRunning = true;
  countingMode = true;

  if (forward) {
    analogWrite(PWM_A_PIN, pwmSpeed);
    analogWrite(PWM_B_PIN, 0);
  } else {
    analogWrite(PWM_A_PIN, 0);
    analogWrite(PWM_B_PIN, pwmSpeed);
  }

  dispenserOn();
  delay(MOTOR_START_DELAY_MS);

  Serial3.printf("[Motor] Started | Speed: %d | Dir: %s\n",
                 pwmSpeed, forward ? "Forward" : "Reverse");
}

void stopMotor(bool error) {
  analogWrite(PWM_A_PIN, 0);
  analogWrite(PWM_B_PIN, 0);
  dispenserOff();
  motorRunning = false;
  countingMode = false;
  delay(MOTOR_STOP_DELAY_MS);

  Serial3.printf("[Motor] Stopped | Error: %s\n", error ? "YES" : "NO");
}

void setMotorSpeed(int pwmSpeed) {
  pwmSpeed = constrain(pwmSpeed, MOTOR_PWM_MIN, MOTOR_PWM_MAX);
  analogWrite(PWM_A_PIN, pwmSpeed);
  Serial3.printf("[Motor] Speed updated: %d\n", pwmSpeed);
}

// ===== DISPENSER =====
void dispenserOn() {
  digitalWrite(DISPENSER_PIN, DISPENSER_ACTIVE_LEVEL);
}

void dispenserOff() {
  digitalWrite(DISPENSER_PIN, !DISPENSER_ACTIVE_LEVEL);
}

// ===== COUNTER LOGIC =====
void resetCounter() {
  objectCount = 0;
  Serial3.println("[Counter] Reset.");
}

bool isTargetReached() {
  return (objectCount >= targetCount);
}

// ===== SENSOR INTERRUPT (จาก FS-N11N / FU-35FA) =====
void sensorTriggerISR() {
  unsigned long now = millis();
  if (now - lastSensorTrigger > SENSOR_DEBOUNCE_TIME_MS) {
    objectCount++;
    lastSensorTrigger = now;
    Serial3.printf("[Sensor] Count = %lu\n", objectCount);
  }
}
