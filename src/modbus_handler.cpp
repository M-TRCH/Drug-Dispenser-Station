#include "modbus_handler.h"
#include "motor_control.h"
#include "system.h"

ModbusRTUServerClass RTUServer;

void setupModbus() {
    if (!RTUServer.begin(rs485, SLAVE_ID, MODBUS_BAUD, SERIAL_8N1)) {
        Serial.println("Failed to start Modbus RTU Server!");
        return;
    }

    Serial.println("[Modbus] RTU Server started");
    RTUServer.configureHoldingRegisters(0, 8);

    RTUServer.holdingRegisterWrite(REG_RUN, 0);
    RTUServer.holdingRegisterWrite(REG_SPEED, 0);
    RTUServer.holdingRegisterWrite(REG_STATUS, 0);
    RTUServer.holdingRegisterWrite(REG_COUNT, 0);
}

void handleModbus() {
    if (RTUServer.poll()) {
        int runCmd = RTUServer.holdingRegisterRead(REG_RUN);
        int speed  = RTUServer.holdingRegisterRead(REG_SPEED);

        if (runCmd == 1) {
            startMotor(speed > 0 ? speed : 2000, true);
            RTUServer.holdingRegisterWrite(REG_STATUS, 1);
        } else {
            stopMotor(false);
            RTUServer.holdingRegisterWrite(REG_STATUS, 0);
        }
    }
    RTUServer.holdingRegisterWrite(REG_COUNT, objectCount & 0xFFFF);
}
