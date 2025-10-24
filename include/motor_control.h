#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <Arduino.h>
#include "system.h"

// Motor PWM control limits
#define MOTOR_PWM_MIN        1000      // Minimum PWM value
#define MOTOR_PWM_MAX        4000      // Maximum PWM value  
#define MOTOR_PWM_DEFAULT    3000      // Default PWM value

// Dispense control settings
#define DISPENSE_TARGET_DEFAULT  10    // Default target count

// Global variables
extern bool motorRunning;              // Motor running state
extern volatile unsigned long objectCount;  // Object counter (interrupt-driven)
extern unsigned int motorVelocity;     // Current motor velocity (PWM)
extern unsigned int dispenseTarget;    // Target dispense count

// Function declarations
void motorControlInit();               // Initialize motor control system
void startMotor(int pwmSpeed, bool forward);  // Start motor with specified speed/direction
void stopMotor(bool error = false);    // Stop motor (with optional error flag)
void resetCounter();                   // Reset object counter

#endif
