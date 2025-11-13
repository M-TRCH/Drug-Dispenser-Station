#include "motor_control.h"

bool flag_motorRunning = false;
MotorMode currentMotorMode = MOTOR_STOP;
int currentMotorSpeed = 0;

void motorControlInit() 
{
    // Configure PWM output pins
    pinMode(PWM_A_PIN, OUTPUT);
    pinMode(PWM_B_PIN, OUTPUT);
    
    // Initialize motor state
    currentMotorMode = MOTOR_STOP;
    currentMotorSpeed = 0;
    
    stopMotor();
    Serial.println("[Motor] Control system initialized");
}

void startMotor(int pwmSpeed, bool forward) 
{
    // Constrain PWM speed to safe limits
    pwmSpeed = constrain(pwmSpeed, MOTOR_PWM_MIN, MOTOR_PWM_MAX);
    
    // Update motor state
    flag_motorRunning = true;
    currentMotorMode = forward ? MOTOR_FORWARD : MOTOR_REVERSE;
    currentMotorSpeed = pwmSpeed;
    
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

void stopMotor(bool brake) 
{
    if (brake && flag_motorRunning) 
    {
        // Dynamic braking: briefly apply reverse current
        if (currentMotorMode == MOTOR_FORWARD) 
        {
            analogWrite(PWM_A_PIN, 0);
            analogWrite(PWM_B_PIN, MOTOR_PWM_DECEL);
            delay(50);  // Brief braking pulse
        } else if (currentMotorMode == MOTOR_REVERSE) 
        {
            analogWrite(PWM_A_PIN, MOTOR_PWM_DECEL);
            analogWrite(PWM_B_PIN, 0);
            delay(50);  // Brief braking pulse
        }
    }
    
    // Stop all PWM outputs
    analogWrite(PWM_A_PIN, 0);
    analogWrite(PWM_B_PIN, 0);
    
    // Update state variables
    flag_motorRunning = false;
    currentMotorMode = MOTOR_STOP;
    currentMotorSpeed = 0;

    Serial.printf("[Motor] Stopped%s\n", brake ? " (with braking)" : "");
}

void changeMotorSpeed(int targetSpeed, int stepSize, int stepDelay)
{
    if (!flag_motorRunning) 
    {
        Serial.println("[Motor] Cannot change speed - motor not running");
        return;
    }
    
    targetSpeed = constrain(targetSpeed, MOTOR_PWM_MIN, MOTOR_PWM_MAX);
    
    Serial.printf("[Motor] Changing speed from %d to %d\n", currentMotorSpeed, targetSpeed);
    
    while (currentMotorSpeed != targetSpeed) 
    {
        if (currentMotorSpeed < targetSpeed) 
        {
            currentMotorSpeed = min(currentMotorSpeed + stepSize, targetSpeed);
        } else 
        {
            currentMotorSpeed = max(currentMotorSpeed - stepSize, targetSpeed);
        }
        
        // Apply new speed while maintaining direction
        if (currentMotorMode == MOTOR_FORWARD) 
        {
            analogWrite(PWM_A_PIN, currentMotorSpeed);
            analogWrite(PWM_B_PIN, 0);
        } else if (currentMotorMode == MOTOR_REVERSE) 
        {
            analogWrite(PWM_A_PIN, 0);
            analogWrite(PWM_B_PIN, currentMotorSpeed);
        }
        
        delay(stepDelay);
    }
    
    Serial.printf("[Motor] Speed change complete: %d\n", currentMotorSpeed);
}

String getMotorStatus()
{
    String status = "Motor Status: ";
    
    if (!flag_motorRunning) 
    {
        status += "STOPPED";
    } else {
        status += "RUNNING | Speed: " + String(currentMotorSpeed) + " | Direction: ";
        status += (currentMotorMode == MOTOR_FORWARD) ? "FORWARD" : "REVERSE";
    }
    
    return status;
}

void testMotorControl()
{
    Serial.println("[Motor] Testing motor control...");
    
    // Test forward direction with gradual acceleration
    Serial.println("[Motor] Test 1: Forward with gradual acceleration");
    startMotor(1000, true);
    changeMotorSpeed(3000, 200, 50);
    delay(2000);
    
    // Test speed reduction
    Serial.println("[Motor] Test 2: Speed reduction");
    changeMotorSpeed(1500, 150, 30);
    delay(1000);
    
    // Test stop with braking
    Serial.println("[Motor] Test 3: Stop with braking");
    stopMotor(true);
    delay(1000);
    
    // Test reverse direction
    Serial.println("[Motor] Test 4: Reverse direction");
    startMotor(2000, false);
    delay(2000);
    
    // Test normal stop
    Serial.println("[Motor] Test 5: Normal stop");
    stopMotor(false);
    
    Serial.println("[Motor] Motor control test complete.");
}
