#include <Arduino.h>
#include "system.h"
#include "modbus.h"

bool motorRunning = false;
unsigned long objectCount = 0;
int pwmValue = 0;

void setup() {
  systemInit();

  // Serial debug ผ่าน USB
  // Serial.begin(9600);
  // Serial.println("System Init OK");

  // Modbus RTU Slave (ผ่าน RS485 A/B)
  setupModbus();
}

void loop() 
{
  RTUServer.poll();

  // Poll Modbus → update motor state
  // handleModbus(motorRunning, pwmValue, objectCount);

  // // Control motor output
  // if (motorRunning) {
  //   analogWrite(PWM_A_PIN, pwmValue);
  //   analogWrite(PWM_B_PIN, 0);
  // } else {
  //   analogWrite(PWM_A_PIN, 0);
  //   analogWrite(PWM_B_PIN, 0);
  // }

  //delay(5);
}
