
#include "dispense.h"

// Global variables
volatile uint32_t rotationCounter = 0;  // Motor rotation counter (1 PPR)
uint32_t targetRotations = 0;           // Target rotation count
bool dispenseActive = false;            // Dispense operation active flag

void _dispenseISR() 
{
    // Increment rotation counter on each sensor trigger (1 PPR)
    rotationCounter++;
    
    // Check if target rotations reached
    if (rotationCounter >= targetRotations && dispenseActive) {
        dispense_stop();
        Serial.printf("[Dispense] Target reached: %d rotations\n", rotationCounter);
    }
}

void dispenseInit()
{   
    // Initialize sensor pin for motor rotation detection (1 PPR)
    pinMode(SEN_1_PIN, INPUT);

    attachInterrupt(digitalPinToInterrupt(SEN_1_PIN), _dispenseISR, RISING);
    
    // Initialize variables
    rotationCounter = 0;
    targetRotations = 0;
    dispenseActive = false;
    
    Serial.println("[Dispense] Motor rotation control initialized (1 PPR)");
}

void dispense_start(uint32_t rotations, int motorSpeed)
{
    // Validate input parameters
    if (rotations == 0 || rotations > DISPENSE_MAX_ROTATIONS) {
        Serial.printf("[Dispense] ERROR: Invalid rotation count %d (max: %d)\n", 
                     rotations, DISPENSE_MAX_ROTATIONS);
        return;
    }
    
    // Stop any current operation
    dispense_stop();
    
    // Reset counter and set target
    rotationCounter = 0;
    targetRotations = rotations;
    dispenseActive = true;
    
    // Start motor
    startMotor(motorSpeed, true);  // Forward direction
    
    Serial.printf("[Dispense] Started: Target=%d rotations, Speed=%d\n", 
                 targetRotations, motorSpeed);
}

void dispense_stop()
{
    // Stop motor
    stopMotor();
    
    // Update state
    dispenseActive = false;
    
    Serial.printf("[Dispense] Stopped at %d/%d rotations\n", 
                 rotationCounter, targetRotations);
}

void dispense_update()
{
    // Auto-stop safety check (in case interrupt doesn't trigger)
    if (dispenseActive && rotationCounter >= targetRotations) {
        dispense_stop();
    }
    
    // Additional safety: stop if motor should be running but flag says it's not
    if (dispenseActive && !flag_motorRunning) {
        Serial.println("[Dispense] WARNING: Motor stopped unexpectedly");
        dispenseActive = false;
    }
}