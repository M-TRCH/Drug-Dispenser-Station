#include "motor_control.h"
#include "system.h"

void motorControlInit() 
{
    pinMode(PWM_A_PIN, OUTPUT);
    pinMode(PWM_B_PIN, OUTPUT);

    analogWriteResolution(MY_PWM_RESOLUTION);
    analogWriteFrequency(MY_PWM_FREQUENCY);

    stopMotor();
    Serial3.println("[Motor] Initialized");
}

// Direction: true = Forward, false = Backward
void startMotor(int speed, bool forward) 
{
    int pwm = constrain(speed, 0, (1 << MY_PWM_RESOLUTION) - 1);

    if (forward) {
        analogWrite(PWM_A_PIN, pwm);
        analogWrite(PWM_B_PIN, 0);
        Serial3.println("[Motor] Direction: FORWARD");
    } else {
        analogWrite(PWM_A_PIN, 0);
        analogWrite(PWM_B_PIN, pwm);
        Serial3.println("[Motor] Direction: REVERSE");
    }

    Serial3.print("[Motor] START, PWM = ");
    Serial3.println(pwm);
    setLEDBuiltIn(true, 100);
}

void stopMotor() 
{
    analogWrite(PWM_A_PIN, 0);
    analogWrite(PWM_B_PIN, 0);

    Serial3.println("[Motor] STOP");
    setLEDBuiltIn(false, 0);
}
