
#ifndef DISPENSE_H
#define DISPENSE_H

#include <Arduino.h>
#include "system.h"

// (0) System constants
#define PWM_MAX             (1 << PWM_RESOLUTION) - 1   // Maximum PWM value based on resolution
#define PWM_LIMIT           (PWM_MAX * 0.98f)
#define RAMP_STEP           25.0f                       // Step size for ramping
#define RAMP_FREQUENCY      1000.0f                     // Frequency of ramping (Hz)
#define RAMP_PERIOD         (1.0f / RAMP_FREQUENCY)     // seconds
#define RAMP_PERIOD_US      (RAMP_PERIOD * 1000000.0f)  // microseconds
#define DISPENSE_MAX_AMOUNT 99                          // Maximum dispense amount

// (1) Variables
extern volatile uint32_t interruptCounter;
extern float currentPWMValue;
extern float setpointPWMValue;
extern uint32_t lastTimeRamping;
extern uint16_t dispenseAmount;
extern bool rampingActive;

// (2) Function declarations
void InterruptEvent();
void dispenseInit();
void dispense_start(int amount, float setpoint);
void dispense_stop();
void dispense_update();
void dispense_debug();

#endif
