
#ifndef DISPENSE_H
#define DISPENSE_H

#include <Arduino.h>
#include "system.h"

// Dispense system constants
#define PWM_MAX             ((1 << MOTOR_PWM_RESOLUTION) - 1)  // Maximum PWM value based on resolution
#define PWM_LIMIT           (PWM_MAX * 0.98f)                  // Safe PWM limit (98% of max)
#define RAMP_STEP           25.0f                              // PWM ramping step size
#define RAMP_FREQUENCY      1000.0f                            // Ramping frequency (Hz)
#define RAMP_PERIOD         (1.0f / RAMP_FREQUENCY)            // Ramping period (seconds)
#define RAMP_PERIOD_US      (RAMP_PERIOD * 1000000.0f)         // Ramping period (microseconds)
#define DISPENSE_MAX_AMOUNT 99                                 // Maximum dispense amount

// Global variables
extern volatile uint32_t interruptCounter;   // Interrupt-based object counter
extern float currentPWMValue;                // Current PWM output value
extern float setpointPWMValue;              // Target PWM setpoint
extern uint32_t lastTimeRamping;            // Last ramping timestamp
extern uint16_t dispenseAmount;             // Target dispense amount
extern bool rampingActive;                  // Ramping state flag

// Function declarations
void InterruptEvent();                      // Interrupt service routine
void dispenseInit();                        // Initialize dispense system
void dispense_start(int amount, float setpoint);  // Start dispensing operation
void dispense_stop();                       // Stop dispensing operation
void dispense_update();                     // Update dispensing process
void dispense_debug();                      // Debug output function

#endif
