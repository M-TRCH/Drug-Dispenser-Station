#include "system.h"
#include "motor_control.h"

// ค่าความเร็ว PWM (0–4095 สำหรับ 12 บิต)
#define TEST_PWM_SPEED 2500  

// Direction: true = Forward, false = Backward
bool direction = true;

void setup() {
  delay(2000);                      // Wait for USB Serial
  Serial.begin(115200);
  while (!Serial);                  // Wait for Serial Monitor
  Serial.println("[Main] Motor Auto Direction Test Start");

  systemInit();
  motorControlInit();
}

void loop() {
  // ===== START =====
  if (direction) {
    Serial.println("[Main] Motor FORWARD for 5 seconds...");
    startMotor(TEST_PWM_SPEED, true);   // Forward
  } else {
    Serial.println("[Main] Motor BACKWARD for 5 seconds...");
    startMotor(TEST_PWM_SPEED, false);  // Backward
  }

  delay(5000);  // Rotate for 5 seconds

  // ===== STOP =====
  Serial.println("[Main] Motor STOP for 5 seconds...");
  stopMotor();
  delay(5000);

  // ===== TOGGLE =====
  direction = !direction;
  Serial.println("[Main] Change direction!");
}
