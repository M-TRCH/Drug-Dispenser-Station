#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <Arduino.h>

// ฟังก์ชันที่ main จะเรียกใช้
void motorControlInit();
void handleMotorCommand();

void startMotor();
void stopMotor();

#endif
