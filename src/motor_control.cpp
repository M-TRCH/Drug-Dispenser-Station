#include "motor_control.h"
#include "system.h"

bool motorRunning = false;
volatile unsigned long objectCount = 0;
unsigned int motorVelocity = MOTOR_PWM_DEFAULT;
unsigned int dispenseTarget = DISPENSE_TARGET_DEFAULT;

void motorControlInit() {
    // Configure PWM output pins
    pinMode(PWM_A_PIN, OUTPUT);
    pinMode(PWM_B_PIN, OUTPUT);
    
    // Set PWM parameters (configured in systemInit)
    // analogWriteResolution and analogWriteFrequency already set
    
    // Initialize motor in stopped state
    stopMotor(false);
    resetCounter();
    
    Serial.println("[Motor] Control system initialized");
}

void startMotor(int pwmSpeed, bool forward) {
    // Constrain PWM speed to safe limits
    pwmSpeed = constrain(pwmSpeed, MOTOR_PWM_MIN, MOTOR_PWM_MAX);
    motorVelocity = pwmSpeed;
    motorRunning = true;
    
    // Set motor direction and speed
    if (forward) {
        analogWrite(PWM_A_PIN, pwmSpeed);
        analogWrite(PWM_B_PIN, 0);
    } else {
        analogWrite(PWM_A_PIN, 0);
        analogWrite(PWM_B_PIN, pwmSpeed);
    }
    
    Serial.printf("[Motor] Started | PWM=%d | Direction=%s\n", 
                  pwmSpeed, forward ? "Forward" : "Reverse");
}

void stopMotor(bool error) {
    // Stop all PWM outputs
    analogWrite(PWM_A_PIN, 0);
    analogWrite(PWM_B_PIN, 0);
    
    // Update state variables
    motorRunning = false;
    motorVelocity = 0;
    
    Serial.printf("[Motor] Stopped%s\n", error ? " (ERROR)" : "");
}

void resetCounter() {
    objectCount = 0;
    Serial.println("[Motor] Counter reset");
}
