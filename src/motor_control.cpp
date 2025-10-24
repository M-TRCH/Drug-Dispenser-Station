#include "motor_control.h"

bool flag_motorRunning = false;

void motorControlInit() 
{
    // Configure PWM output pins
    pinMode(PWM_A_PIN, OUTPUT);
    pinMode(PWM_B_PIN, OUTPUT);
    
    stopMotor();
    Serial.println("[Motor] Control system initialized");
}

void startMotor(int pwmSpeed, bool forward) 
{
    // Constrain PWM speed to safe limits
    pwmSpeed = constrain(pwmSpeed, MOTOR_PWM_MIN, MOTOR_PWM_MAX);
    flag_motorRunning = true;
    
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

void stopMotor() 
{
    // Stop all PWM outputs
    analogWrite(PWM_A_PIN, 0);
    analogWrite(PWM_B_PIN, 0);
    
    // Update state variables
    flag_motorRunning = false;

    Serial.printf("[Motor] Stopped\n");
}

void testMotorControl()
{
    Serial.println("[Motor] Testing motor control...");
    
    startMotor(2000, true);  // Start motor forward at PWM 2000
    delay(2000);             // Run for 2 seconds
    
    stopMotor();             // Stop motor
    delay(1000);             // Wait for 1 second
    
    startMotor(3000, false); // Start motor reverse at PWM 3000
    delay(2000);             // Run for 2 seconds
    
    stopMotor();             // Stop motor
    
    Serial.println("[Motor] Motor control test complete.");
}
