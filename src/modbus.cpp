#include "modbus.h"

bool modbusInit(HardwareSerial& serialPort) {
  // เริ่ม Modbus RTU Slave
  serialPort.begin(9600, SERIAL_8N1);
  if (!ModbusRTUServer.begin(SLAVE_ID, serialPort)) {
    return false; // fail
  }

  // จอง holding register 0–7
  ModbusRTUServer.configureHoldingRegisters(0, 8);

  // ค่าเริ่มต้น
  ModbusRTUServer.holdingRegisterWrite(REG_RUN, 0);
  ModbusRTUServer.holdingRegisterWrite(REG_SPEED, 2000); // default speed
  ModbusRTUServer.holdingRegisterWrite(REG_COUNT, 0);
  ModbusRTUServer.holdingRegisterWrite(REG_STATUS, 0);

  return true;
}

void modbusPoll() {
  ModbusRTUServer.poll();
}

int modbusGetRunCommand() {
  return ModbusRTUServer.holdingRegisterRead(REG_RUN);
}

int modbusGetSpeed() {
  return ModbusRTUServer.holdingRegisterRead(REG_SPEED);
}

void modbusSetStatus(int status) {
  ModbusRTUServer.holdingRegisterWrite(REG_STATUS, status);
}

void modbusSetCount(int count) {
  ModbusRTUServer.holdingRegisterWrite(REG_COUNT, count);
}
