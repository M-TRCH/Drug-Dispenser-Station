
#ifndef DISPENSE_H
#define DISPENSE_H

#include <Arduino.h>
#include "system.h"
#include "motor_control.h"

// Dispense system constants
#define DISPENSE_MAX_ROTATIONS 99          // Maximum rotation count

// Home system constants
#define HOME_MAX_ROTATIONS 2                   // Home calibration rotations
#define HOME_SEEK_TIMEOUT 10000                // Home seek timeout (10 seconds)

// Global variables
extern volatile uint32_t rotationCounter;   // Motor rotation counter (1 PPR)
extern uint32_t targetRotations;            // Target rotation count
extern bool dispenseActive;                 // Dispense operation active flag
extern bool homeActive;                     // Home operation active flag
extern bool homeCompleted;                  // Home operation completed flag
extern bool homeSeeking;                    // Home seeking operation active flag
extern unsigned long homeSeekStartTime;     // Home seek start time (for timeout)
extern uint8_t lastErrorCode;               // Last error code

// Function declarations
void dispenseInit();                      // Initialize dispense system
void dispense_start(uint32_t rotations, int motorSpeed = MOTOR_PWM_DEFAULT); // Start dispensing operation  
void dispense_stop();                     // Stop dispensing operation
void dispense_update();                   // Update dispensing process (check rotation count)
void home_start(uint32_t rotations, int motorSpeed = MOTOR_PWM_SLOW); // Start home operation with specified rotations and speed
void home_stop();                         // Stop home operation
bool is_home_completed();                 // Check if home operation is completed
void home_seek_start(int motorSpeed = MOTOR_PWM_SLOW); // Start seeking home sensor
void home_seek_stop();                    // Stop seeking home sensor
void home_seek_update();                  // Update seeking process (check sensor and timeout)
void clear_error_code();                  // Clear last error code

// Internal interrupt handler (do not call directly)
void _dispenseISR();                      // Motor rotation interrupt service routine

#endif
