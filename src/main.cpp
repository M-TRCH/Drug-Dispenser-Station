#include <Arduino.h>
#include "system.h"
#include "modbus_handler.h"

bool motorRunning = false;
unsigned long objectCount = 0;
int pwmValue = 0;

void setup() 
{
    systemInit();
    setupModbus();
}

void loop()
{
    if (RTUServer.poll())
    {
        Serial3.println("Modbus request received");
    }
}


