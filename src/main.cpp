#include "system.h"
#include "motor_control.h"

// config PWM (0–4095 สำหรับ 12 bits)
#define TEST_PWM_SPEED 2500  

void setup() {
  delay(2000);                      
  Serial.begin(115200);

  systemInit();
  motorControlInit();
}

void loop() {
  Serial.println("Piwww");
  startMotor(TEST_PWM_SPEED);
  delay(5000);

  stopMotor();
  delay(5000);
}
