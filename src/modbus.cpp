#include "modbus.h"
#include "system.h"

void setupModbus() {
  // RS485 ใช้ Serial3 (PA9, PA10)
  if (!ModbusRTUServer.begin(SLAVE_ID, Serial3)) {
    Serial.println("Failed to start Modbus RTU Server!");
    while (1);
  }

  // สร้าง Holding Registers (0–7)
  ModbusRTUServer.configureHoldingRegisters(0, 8);

  // ค่าเริ่มต้น
  ModbusRTUServer.holdingRegisterWrite(REG_RUN, 0);
  ModbusRTUServer.holdingRegisterWrite(REG_SPEED, 3000);
  ModbusRTUServer.holdingRegisterWrite(REG_COUNT, 0);
  ModbusRTUServer.holdingRegisterWrite(REG_STATUS, 0);
}

void handleModbus(bool &motorRunning, int &pwmValue, unsigned long objectCount) {
  ModbusRTUServer.poll();

  int runCmd = ModbusRTUServer.holdingRegisterRead(REG_RUN);
  int speed  = ModbusRTUServer.holdingRegisterRead(REG_SPEED);

  if (runCmd == 1) {
    if (speed == 0) speed = 3000; // default speed
    pwmValue = speed;

    if (!motorRunning) {
      motorRunning = true;
      ModbusRTUServer.holdingRegisterWrite(REG_STATUS, 1);
      Serial.println("Motor RUN via Modbus");
    }
  } else {
    pwmValue = 0;
    if (motorRunning) {
      motorRunning = false;
      ModbusRTUServer.holdingRegisterWrite(REG_STATUS, 0);
      Serial.println("Motor STOP via Modbus");
    }
  }

  // update object count
  ModbusRTUServer.holdingRegisterWrite(REG_COUNT, objectCount);
}
