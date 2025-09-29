
#include "modbus.h"
#include "system.h"
#include <ArduinoModbus.h>

ModbusRTUServerClass RTUServer;

void modbusInit(int id) {
        // id = SLAVE_ID;

        // #if MODBUS_OUTPUT == MODBUS_SERIAL
        // RTUServer.begin(rs485, id, MODBUS_BAUD, SERIAL_8N1);
        // #elif MODBUS_OUTPUT == MODBUS_SERIAL
        // RTUServer.begin(rs485_3, id, MODBUS_BAUD, SERIAL_8N1);
        // #endif

        // RTUServer.configureCoils(0x00, COIL_NUM);
        // RTUServer.configureDiscreteInputs(0x00, DISCRETE_INPUT_NUM);
        // RTUServer.configureHoldingRegisters(0x00, HOLDING_REGISTER_NUM);
        // RTUServer.configureInputRegisters(0x00, INPUT_REGISTER_NUM);
}

void setupModbus() 
{
        if (!RTUServer.begin(rs485, SLAVE_ID, MODBUS_BAUD, SERIAL_8N1)) {
                Serial.println("Failed to start Modbus RTU Server!");
                while (1);
        }
        RTUServer.configureHoldingRegisters(0, 8);
        // RTUServer.holdingRegisterWrite(REG_BAUD_RATE, 0);
        // RTUServer.holdingRegisterWrite(REG_IDENTIFIER, 0);
        // RTUServer.holdingRegisterWrite(REG_STATUS, 0);
}

void handleModbus(bool &motorRunning, int &pwmValue, unsigned long objectCount) {
        RTUServer.poll();
        int runCmd = RTUServer.holdingRegisterRead(REG_RUN);
        int speed  = RTUServer.holdingRegisterRead(REG_SPEED);
        int baud   = RTUServer.holdingRegisterRead(REG_BAUD_RATE);
        int id     = RTUServer.holdingRegisterRead(REG_IDENTIFIER);


        if (runCmd == 1) {
                if (speed == 0) speed = 3000;
                pwmValue = speed;
                if (!motorRunning) {
                        motorRunning = true;
                        RTUServer.holdingRegisterWrite(REG_STATUS, 1);
                        Serial.println("Motor RUN via Modbus");
                }
        } else {
                pwmValue = 0;
                if (motorRunning) {
                        motorRunning = false;
                        RTUServer.holdingRegisterWrite(REG_STATUS, 0);
                        Serial.println("Motor STOP via Modbus");
                }
        }
        RTUServer.holdingRegisterWrite(REG_COUNT, objectCount & 0xFFFF);
        
}