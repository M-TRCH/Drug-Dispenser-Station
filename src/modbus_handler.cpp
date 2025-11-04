#include "modbus_handler.h"

ModbusRTUServerClass rtu;

void modbusInit() 
{
    // Initialize Modbus RTU server
    if (!rtu.begin(rs485, MODBUS_SLAVE_ID, RS485_BAUDRATE, SERIAL_8N1)) {
        Serial.println("[Modbus] ERROR: Failed to start RTU Server!");
        return;
    }

    // Configure holding registers
    rtu.configureHoldingRegisters(0, MODBUS_REGISTER_COUNT);

    // Initialize register values
    rtu.holdingRegisterWrite(ADDR_REG_SPEED, MOTOR_PWM_DEFAULT);  // Default speed
    rtu.holdingRegisterWrite(ADDR_REG_DISP, 0);                    

    Serial.printf("[Modbus] RTU Server started (Slave ID: %d)\n", MODBUS_SLAVE_ID);
}
