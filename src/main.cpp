
/**
 * @file main.cpp
 * @brief Drug Dispenser Station - Main Application
 * @author M.Teerachot, ijaoMxy
 * @date 2025
 * 
 * This is the main application file for the drug dispenser station system.
 * It initializes all subsystems and manages the main control loop.
 */

#include "system.h"
#include "motor_control.h"
#include "dispense.h"
// #include "modbus_handler.h"
// #include "sensor_handler.h"

void setup() 
{
#ifdef SYSTEM_H
    systemInit();        // Core system initialization
#endif

#ifdef MOTOR_CONTROL_H
    motorControlInit();  // Motor control system
#endif

#ifdef DISPENSE_H
    dispenseInit();      // Dispense system initialization
#endif
}

void loop() 
{
    // Update dispense system (safety checks)
    dispense_update();

    if (SW_START_PRESSING) 
    {
        dispense_start(10, MOTOR_PWM_DECEL); // Start dispensing 10 rotations
        delay(500); // Debounce delay
    }
}
