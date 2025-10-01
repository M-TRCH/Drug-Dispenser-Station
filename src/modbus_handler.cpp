#include "modbus_handler.h"
#include "motor_control.h"

ModbusRTUServerClass RTUServer;

void setupModbus() {
    if (!RTUServer.begin(rs485, SLAVE_ID, MODBUS_Baud, SERIAL_8N1)) {
        Serial3.println("Failed to start Modbus RTU Server!");
    } else {
        Serial3.println("Modbus RTU Server started");
    }

    RTUServer.configureHoldingRegisters(0, 8);

    RTUServer.holdingRegisterWrite(REG_RUN, 0);
    RTUServer.holdingRegisterWrite(REG_SPEED, 0);
    RTUServer.holdingRegisterWrite(REG_STATUS, 0);
    RTUServer.holdingRegisterWrite(REG_COUNT, 0);
}

void handleModbus(unsigned long objectCount) 
{
    // Poll for Modbus requests
    if (RTUServer.poll()) 
    {
        int runCmd = RTUServer.holdingRegisterRead(REG_RUN);
        int speed  = RTUServer.holdingRegisterRead(REG_SPEED);

        if (runCmd == 1) 
        {
            startMotor(speed > 0 ? speed : 2000);
            RTUServer.holdingRegisterWrite(REG_STATUS, 1);
            Serial3.print("Motor START, Speed = ");
            Serial3.println(speed);
        } 
        else if (runCmd == 0) 
        {
            stopMotor();
            RTUServer.holdingRegisterWrite(REG_STATUS, 0);
            Serial3.println("Motor STOP");
        }
    }

    // Update counter register
    RTUServer.holdingRegisterWrite(REG_COUNT, objectCount & 0xFFFF);
}
