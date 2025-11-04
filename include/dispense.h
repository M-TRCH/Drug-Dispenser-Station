
#ifndef DISPENSE_H
#define DISPENSE_H

#include <Arduino.h>
#include "system.h"
#include "motor_control.h"

// Dispense system constants
#define DISPENSE_MAX_ROTATIONS 99          // Maximum rotation count

// Global variables
extern volatile uint32_t rotationCounter;   // Motor rotation counter (1 PPR)
extern uint32_t targetRotations;            // Target rotation count
extern bool dispenseActive;                 // Dispense operation active flag
extern bool homePositionFound;              // Home position status

// Function declarations
void dispenseInit();                      // Initialize dispense system
void dispense_start(uint32_t rotations, int motorSpeed = MOTOR_PWM_DEFAULT); // Start dispensing operation  
void dispense_stop();                     // Stop dispensing operation
void dispense_update();                   // Update dispensing process (check rotation count)

// Additional functions for modbus integration
void resetCounter();                      // Reset rotation counter
void returnToHome();                      // Return to home position
bool isAtTargetPosition(float target);    // Check if at target position
float getCurrentPosition();               // Get current position

// ===== PHASE 1: SENSOR HEALTH MONITORING =====
void monitorSensorHealth();               // Monitor sensor health status
void printSensorHealthStatus();           // Print sensor health statistics

// ===== ISR CONTROL FUNCTIONS =====
void disableRotationISR();                // Disable rotation counting ISR
void enableRotationISR();                 // Enable rotation counting ISR

// Internal interrupt handler (do not call directly)
void _dispenseISR();                      // Motor rotation interrupt service routine

#endif
