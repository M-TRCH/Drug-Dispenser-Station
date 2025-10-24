
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
    
    // Stop dispensing when target amount is reached
    if (interruptCounter >= dispenseAmount) {
        dispense_stop();
        dispense_debug();
    }
}

void dispenseInit()
{   
    // Initialize sensor pin as input with pull-up resistor
    pinMode(SEN_1_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(SEN_1_PIN), InterruptEvent, RISING);
    Serial.println("[Dispense] System initialized");
}

void dispense_start(int amount, float setpoint)
{
    // Reset system state
    interruptCounter = 0;
    lastTimeRamping = micros();
    currentPWMValue = 0.0f;
    
    // Set target parameters with constraints
    setpointPWMValue = constrain(fabs(setpoint), 0.0f, PWM_LIMIT);
    dispenseAmount = constrain(abs(amount), 0, DISPENSE_MAX_AMOUNT);
    rampingActive = true;
    
    Serial.printf("[Dispense] Started: Amount=%d, Setpoint=%.1f\n", dispenseAmount, setpointPWMValue);
}

void dispense_stop()
{
    // Stop all PWM outputs
    analogWrite(PWM_A_PIN, 0);
    analogWrite(PWM_B_PIN, 0);
    analogWrite(PWM_C_PIN, 0);
    
    // Disable ramping
    rampingActive = false;
    currentPWMValue = 0.0f;
    
    Serial.println("[Dispense] Stopped");
}

void dispense_update()
{
    uint32_t currentTime = micros();
    
    // Check if ramping update is needed
    if (rampingActive && (currentTime - lastTimeRamping >= RAMP_PERIOD_US)) {
        lastTimeRamping = currentTime;
        
        // Update PWM ramping if setpoint is valid
        if (setpointPWMValue > 0) {
            currentPWMValue += RAMP_STEP;
            if (currentPWMValue >= setpointPWMValue) {
                currentPWMValue = setpointPWMValue;
            }
            
            // Apply PWM to motor pins
            analogWrite(PWM_A_PIN, (int)currentPWMValue);
            analogWrite(PWM_B_PIN, 0);  // Reverse direction (inactive)
        }
        
        dispense_debug();
    }
}
 
void dispense_debug()
{
    Serial3.printf("Target: %d\tCount: %d\tPWM: %.1f\n", 
                   dispenseAmount, interruptCounter, currentPWMValue);
}