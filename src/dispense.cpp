
#include "dispense.h"

volatile uint32_t interruptCounter = 0;
float currentPWMValue = 0.0f;
float setpointPWMValue = 0.0f;
uint32_t lastTimeRamping = 0;
uint16_t dispenseAmount = 0;
bool rampingActive = false;

void InterruptEvent() 
{
    interruptCounter++;
    if (interruptCounter >= dispenseAmount)
    {
        // Stop dispensing when the target amount is reached
        dispense_stop();
    }
}

void dispenseInit()
{   
     // Initialize sensor pin as input
    pinMode(SEN_1_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(SEN_1_PIN), InterruptEvent, RISING); 
}

void dispense_start(int amount, float setpoint)
{
    interruptCounter = 0;       // Reset interrupt counter
    lastTimeRamping = micros(); // Initialize last time for ramping
    currentPWMValue = 0.0f;     // Initialize current PWM value
    setpointPWMValue = constrain(fabs(setpoint), 0.0f, PWM_LIMIT); // Set the desired PWM limit
    dispenseAmount = constrain(abs(amount), 0, DISPENSE_MAX_AMOUNT);
    rampingActive = true;       // Start ramping
}

void dispense_stop()
{
    analogWrite(PWM_A_PIN, 0);
    analogWrite(PWM_B_PIN, 0);
    analogWrite(PWM_C_PIN, 0);

    rampingActive = false;  // Stop ramping
}

void dispense_update()
{
    uint32_t currentTime = micros();
    if (rampingActive && (currentTime - lastTimeRamping >= RAMP_PERIOD_US))
    {
        lastTimeRamping = currentTime;
        Serial3.print(dispenseAmount); // Debug output
        Serial3.print("\t");
        Serial3.println(interruptCounter);

        if (setpointPWMValue != 0)
        {
            currentPWMValue += RAMP_STEP;
            if (currentPWMValue >= setpointPWMValue)  currentPWMValue = setpointPWMValue;

            analogWrite(PWM_A_PIN, (int)currentPWMValue);  // Example PWM signal on PWM_A_PIN
            analogWrite(PWM_B_PIN, 0);                  // Example PWM signal on PWM_B_PIN
        }
    }
}
