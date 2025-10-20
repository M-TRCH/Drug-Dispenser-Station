#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <Arduino.h>

// === Motor PWM Limits ===
#define MOTOR_PWM_MIN        1000
#define MOTOR_PWM_MAX        4000
#define MOTOR_PWM_DEFAULT    3000

// === Counter / Dispense ===
#define DISPENSE_TARGET_DEFAULT  10

extern bool motorRunning;
extern volatile unsigned long objectCount;
extern unsigned int motorVelocity;
extern unsigned int dispenseTarget;

// === Functions ===
void motorControlInit();
void startMotor(int pwmSpeed, bool forward);
void stopMotor(bool error = false);
void resetCounter();

#endif
