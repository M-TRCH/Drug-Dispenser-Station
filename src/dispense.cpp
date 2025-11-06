
#include "dispense.h"

// Global variables
volatile uint32_t rotationCounter = 0;  // Motor rotation counter (1 PPR)
uint32_t targetRotations = 0;           // Target rotation count
bool dispenseActive = false;            // Dispense operation active flag
bool homeActive = false;                // Home operation active flag
bool homeCompleted = false;             // Home operation completed flag
bool homeSeeking = false;               // Home seeking operation active flag
unsigned long homeSeekStartTime = 0;    // Home seek start time (for timeout)
uint8_t lastErrorCode = 0;              // Last error code

void _dispenseISR() 
{
    // Increment rotation counter on each sensor trigger (1 PPR)
    rotationCounter++;
    
    // Check if home seeking is active - sensor found!
    if (homeSeeking) {
        home_seek_stop();
        Serial.println("[Home Seek] Sensor detected - Home position found!");
        return;
    }
    
    // Check if home operation is active and completed
    if (homeActive && rotationCounter >= HOME_MAX_ROTATIONS) 
    {
        home_stop();
        Serial.printf("[Home] Completed: %d rotation\n", rotationCounter);
        return;
    }
    
    // Check if target rotations reached for dispense operation
    if (rotationCounter >= targetRotations && dispenseActive) 
    {
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
    homeActive = false;
    homeCompleted = false;
    homeSeeking = false;
    homeSeekStartTime = 0;
    lastErrorCode = 0;
    
    Serial.println("[Dispense] Motor rotation control initialized (1 PPR)");
}

void dispense_start(uint32_t rotations, int motorSpeed)
{
    // Validate input parameters
    if (rotations == 0 || rotations > DISPENSE_MAX_ROTATIONS) 
    {
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
    if (dispenseActive && rotationCounter >= targetRotations) 
    {
        dispense_stop();
    }
    
    // Auto-stop safety check for home operation
    if (homeActive && rotationCounter >= HOME_MAX_ROTATIONS) 
    {
        home_stop();
    }
    
    // Update home seeking process
    if (homeSeeking) {
        home_seek_update();
    }
    
    // Additional safety: stop if motor should be running but flag says it's not
    if ((dispenseActive || homeActive) && !flag_motorRunning)
    {
        Serial.println("[System] WARNING: Motor stopped unexpectedly");
        dispenseActive = false;
        homeActive = false;
    }
}

void home_start(uint32_t rotations, int motorSpeed)
{
    // Validate input parameters
    if (rotations == 0 || rotations > DISPENSE_MAX_ROTATIONS) 
    {
        Serial.printf("[Home] ERROR: Invalid rotation count %d (max: %d)\n", 
                     rotations, DISPENSE_MAX_ROTATIONS);
        return;
    }
    
    // Stop any current operation
    dispense_stop();
    home_stop();
    
    // Reset counter and set home target
    rotationCounter = 0;
    targetRotations = rotations;
    homeActive = true;
    homeCompleted = false;

    // Start motor at specified speed
    startMotor(motorSpeed, true);  // Forward direction

    Serial.printf("[Home] Started: Target=%d rotations, Speed=%d\n", 
                 targetRotations, motorSpeed);
}

void home_stop()
{
    // Stop motor
    stopMotor();
    
    // Update state
    homeActive = false;
    homeCompleted = true;
    
    Serial.printf("[Home] Completed at %d rotation\n", rotationCounter);
}

bool is_home_completed()
{
    return homeCompleted;
}

void home_seek_start(int motorSpeed)
{
    // Stop any current operation
    dispense_stop();
    home_stop();
    home_seek_stop();
    
    // Clear any previous error
    clear_error_code();
    
    // Reset counter and set seeking mode
    rotationCounter = 0;
    homeSeeking = true;
    homeCompleted = false;
    homeSeekStartTime = millis();
    
    // Start motor at specified speed (usually slow for accuracy)
    startMotor(motorSpeed, true);  // Forward direction
    
    Serial.println("[Home Seek] Started: Seeking home sensor...");
}

void home_seek_stop()
{
    // Stop motor
    stopMotor();
    
    // Update state
    homeSeeking = false;
    homeCompleted = true;
    
    Serial.println("[Home Seek] Completed: Home sensor found!");
}

void home_seek_update()
{
    // Check for timeout
    if (millis() - homeSeekStartTime > HOME_SEEK_TIMEOUT) {
        Serial.println("[Home Seek] ERROR: Timeout - Home sensor not found!");
        stopMotor();  // Stop motor immediately
        homeSeeking = false;
        homeCompleted = false; // Mark as failed
        lastErrorCode = 5; // ERR_HOME_SEEK_TIMEOUT
        return;
    }
    
    // Additional safety: stop if motor should be running but flag says it's not
    if (homeSeeking && !flag_motorRunning) {
        Serial.println("[Home Seek] WARNING: Motor stopped unexpectedly");
        homeSeeking = false;
        homeCompleted = false;
    }
}

void clear_error_code()
{
    lastErrorCode = 0;
}