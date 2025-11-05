
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

// Dispense configuration
#define DISPENSE_ROTATIONS 10      // Number of rotations for dispense
#define HOME_ROTATIONS 1          // Number of rotations for home calibration

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

    // Debug: Status print every 2 seconds
    static unsigned long lastDebugTime = 0;
    if (millis() - lastDebugTime > 2000) 
    {
        Serial.printf("[Debug] SW_CALC: %d, SW_START: %d\n", 
                     digitalRead(SW_CALC_PIN), digitalRead(SW_START_PIN));
        lastDebugTime = millis();
    }

    // HOME button (SW_CALC) - HOME TO CALIBRATE THEN DISPENSE
    if (SW_CALC_PRESSING) 
    {
    {
        Serial.println("[Main] Home button pressed - Starting home sequence");
        
        // Step 1: ทำ HOME (หมุน 1 รอบเพื่อ calibrate)
        home_start(1, MOTOR_PWM_SLOW);
        
        // HOLD จนกว่า HOME จะเสร็จ
        while (homeActive) 
        {
            dispense_update(); // Continue safety checks
            delay(500); // Polling delay
        }
        
        // Step 2: START Dispense 
        if (is_home_completed()) 
        {
            Serial.println("[Main] Home completed - Starting dispense sequence");
            dispense_start(DISPENSE_ROTATIONS, MOTOR_PWM_DEFAULT); // Start dispensing with default speed
        }
        
        delay(500); // Debounce delay
    }

    // START button (SW_START) - กดเพื่อจ่ายยาโดยตรง (ไม่ต้อง HOME)
    if (SW_START_PRESSING) 
    {
        Serial.println("[Main] Start button pressed - Direct dispense");
        dispense_start(DISPENSE_ROTATIONS, MOTOR_PWM_DEFAULT); // Start dispensing with default speed
        delay(500); // Debounce delay
    }
    }
}
