
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
    
    // sensorInit();        // Sensor handling system
    // setupModbus();       // Modbus communication

    // Serial.println("[Main] Drug Dispenser Station Ready!");
    // Serial.println("[Main] Waiting for Modbus commands...");
}

void loop() 
{

    if (SW_CALC_PRESSING) 
    {
        testMotorControl();
        Serial.println("[Main] Start button pressed");
        // Add start button handling logic here
        delay(500); // Debounce delay
    }

    // // Main control loop
    // handleModbus();      // Process Modbus communication
    // handleSensorLogic(); // Process sensor logic (if needed)
    
    // // Small delay to prevent Modbus timeout and reduce CPU load
    // delay(20);
}
