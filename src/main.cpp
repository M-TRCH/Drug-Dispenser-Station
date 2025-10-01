#include <Arduino.h>
#include "system.h"
#include "motor_control.h"

void setup() 
{
  systemInit();
  motorControlInit();   
  Serial3.println("System Initialized. Waiting for START...");
}

void loop() 
{
  handleMotorCommand(); 
}
