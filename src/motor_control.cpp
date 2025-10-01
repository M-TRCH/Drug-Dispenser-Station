#include "motor_control.h"
#include "system.h"

bool motorRunning = false;

void motorControlInit() 
{
  pinMode(SW_START_PIN, INPUT_PULLUP);
  pinMode(SW_STOP_PIN, INPUT_PULLUP);
  pinMode(PWM_A_PIN, OUTPUT);
  pinMode(PWM_B_PIN, OUTPUT);

  analogWriteResolution(MY_PWM_RESOLUTION);
  analogWriteFrequency(MY_PWM_FREQUENCY);

  stopMotor();
}

// Botton handling
void handleMotorCommand() 
{
  // START
  if (SW_START_PRESSING && !motorRunning) 
  {
    startMotor();
    motorRunning = true;
    Serial3.println("Motor Started by START button");
    delay(300); 
  }

  // STOP
  if (SW_STOP_PRESSING && motorRunning) 
  {
    stopMotor();
    motorRunning = false;
    Serial3.println("Motor Stopped by STOP button");
    delay(300); 
  }
}

// ====== functions to control motor ======
void startMotor() 
{
  analogWrite(PWM_A_PIN, 3000); // (0–4095) 12-bit
  analogWrite(PWM_B_PIN, 0);
  setLEDBuiltIn(true, false, false);
}

void stopMotor() 
{
  analogWrite(PWM_A_PIN, 0);
  analogWrite(PWM_B_PIN, 0);
  setLEDBuiltIn(false, false, false);
}
