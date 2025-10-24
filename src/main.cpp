
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
#include "modbus_handler.h"

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
    
#ifdef MODBUS_HANDLER_H
    modbusInit();        // Modbus communication initialization
#endif
}

void loop() 
{
    // Update dispense system (safety checks)
    dispense_update();

    // Handle Modbus communication
    if (rtu.poll())
    {
        int speed = (int)rtu.holdingRegisterRead(ADDR_REG_SPEED);
        int dispense = (int)rtu.holdingRegisterRead(ADDR_REG_DISP);

        if (speed != 0 && dispense != 0) 
        {
            dispense_start(dispense, speed); // Start dispensing command
            rtu.holdingRegisterWrite(ADDR_REG_DISP, 0); // Reset dispense command
            Serial.printf("[Main] Dispense command received: %d rotations at speed %d\n", dispense, speed);
        } 
    }
}
