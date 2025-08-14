
#include <Arduino.h>
#include "system.h"
#include "dispense.h"

void setup() 
{
    #ifdef SYSTEM_H
        systemInit(); 
    #endif

    #ifdef DISPENSE_H
        dispenseInit();
    #endif

    dispense_stop(); // Ensure motors are stopped at startup
    // while (!SW_START_PRESSING)
    // {
    //     setLEDBuiltIn(false, true, false);  
    // }
    setLEDBuiltIn(true, false, false);  // Set RUN LED on when start button is pressed
}

void loop() 
{
    if (Serial3.available())
    {
        if (Serial3.find("#"))
        {
            // Parse and execute command
            int amount = Serial3.parseInt();
            int setpoint = Serial3.parseInt();
            if (amount < 0 || amount > DISPENSE_MAX_AMOUNT)
            {
                dispenseAmount = interruptCounter;
            }
            else
            {
                dispense_start(amount, setpoint);
            }
        }
    }

    dispense_update();  // Call the dispense update function
}
