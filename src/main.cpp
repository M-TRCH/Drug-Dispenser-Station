#include <Arduino.h>
#include "system.h"
#include "modbus.h"

const int MOTOR_PWM = PWM_A_PIN;
bool motorRunning = false;

void setup() {
  systemInit();

  pinMode(MOTOR_PWM, OUTPUT);

  Serial.begin(115200); // debug USB
  Serial.println("System Init...");

  if (!modbusInit(Serial3)) {
    Serial.println("Modbus Init Failed!");
    while (1);
  }
  Serial.println("Modbus Ready.");
}

void loop() {
  modbusPoll(); // handle Modbus request

  int runCmd = modbusGetRunCommand();
  int speed  = modbusGetSpeed();

  if (runCmd == 1 && !motorRunning) {
    if (speed == 0) speed = 2000; // default speed
    analogWrite(MOTOR_PWM, speed);
    motorRunning = true;
    modbusSetStatus(1); // Running
    Serial.println("Motor START");
  }
  else if (runCmd == 0 && motorRunning) {
    analogWrite(MOTOR_PWM, 0);
    motorRunning = false;
    modbusSetStatus(0); // Idle
    Serial.println("Motor STOP");
  }
}