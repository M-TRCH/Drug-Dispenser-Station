#include "motor_control.h"
#include "system.h"

bool motorRunning = false;
volatile unsigned long objectCount = 0;
unsigned int motorVelocity = MOTOR_PWM_DEFAULT;
unsigned int dispenseTarget = DISPENSE_TARGET_DEFAULT;

void motorControlInit() {
    pinMode(PWM_A_PIN, OUTPUT);
    pinMode(PWM_B_PIN, OUTPUT);
    analogWriteResolution(MOTOR_PWM_RESOLUTION);
    analogWriteFrequency(MOTOR_PWM_FREQUENCY);
    stopMotor(false);
    Serial.println("[Motor] Control initialized");
}

void startMotor(int pwmSpeed, bool forward) {
    pwmSpeed = constrain(pwmSpeed, MOTOR_PWM_MIN, MOTOR_PWM_MAX);
    motorRunning = true;
    if (forward) {
        analogWrite(PWM_A_PIN, pwmSpeed);
        analogWrite(PWM_B_PIN, 0);
    } else {
        analogWrite(PWM_A_PIN, 0);
        analogWrite(PWM_B_PIN, pwmSpeed);
    }
    Serial.printf("[Motor] START | PWM=%d | Dir=%s\n", pwmSpeed, forward ? "FWD" : "REV");
}

void stopMotor(bool error) {
    analogWrite(PWM_A_PIN, 0);
    analogWrite(PWM_B_PIN, 0);
    motorRunning = false;
    Serial.println("[Motor] STOP");
}

void resetCounter() {
    objectCount = 0;
}
