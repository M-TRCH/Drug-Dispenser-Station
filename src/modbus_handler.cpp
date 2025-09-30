
#include "modbus_handler.h"
#include "system.h"
#include <ArduinoModbus.h>

ModbusRTUServerClass RTUServer;

void setupModbus() 
{
    if (!RTUServer.begin(rs485, SLAVE_ID, MODBUS_Baud, SERIAL_8N1)) 
    {
        Serial.println("Failed to start Modbus RTU Server!");
    }
    RTUServer.configureHoldingRegisters(0, 8);
}

void handleModbus(bool &motorRunning, int &pwmValue, unsigned long objectCount) 
{
    if (RTUServer.poll()) 
    {
        int runCmd = RTUServer.holdingRegisterRead(REG_RUN);
        int speed  = 150;// Server.holdingRegisterRead(REG_SPEED);
        int baud   = RTUServer.holdingRegisterRead(REG_BAUD_RATE);
        int id     = RTUServer.holdingRegisterRead(REG_IDENTIFIER);

        if (runCmd == 1) 
        {
            pwmValue = speed;
            if (!motorRunning)
            {
                motorRunning = true;
                RTUServer.holdingRegisterWrite(REG_STATUS, 1);
                Serial.println("Motor RUN via Modbus");
            }
        }
        else
        {
            pwmValue = 0;
            if (motorRunning)
            {
                motorRunning = false;
                RTUServer.holdingRegisterWrite(REG_STATUS, 0);
                Serial.println("Motor STOP via Modbus");
            }
        }
    }
    RTUServer.holdingRegisterWrite(REG_COUNT, objectCount & 0xFFFF);    
}