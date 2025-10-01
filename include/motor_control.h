#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <Arduino.h>

// function prototypes
void motorControlInit();   // Initialize motor control
void startMotor(int speed); // Start motor with PWM value
void stopMotor();          

#endif
