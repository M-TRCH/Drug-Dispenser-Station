#ifndef MODBUS_HANDLER_H
#define MODBUS_HANDLER_H

#include <Arduino.h>
#include <ArduinoModbus.h>

// ===== Modbus Config =====
#define SLAVE_ID     2
#define REG_RUN      0   // 0 = Stop, 1 = Run
#define REG_SPEED    1   // PWM Speed
#define REG_COUNT    2   // Object Count
#define REG_STATUS   3   // 0 = Idle, 1 = Running

void setupModbus();
void handleModbus(bool &motorRunning, int &pwmValue, unsigned long objectCount);

#endif
