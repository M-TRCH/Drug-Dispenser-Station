#include <Arduino.h>
#include "system.h"
#include "motor_control.h"
#include "modbus_handler.h"

unsigned long objectCount = 0;

void setup() {
    systemInit();
    motorControlInit();
    setupModbus();
    Serial3.println("System Ready (Modbus Test Mode)");
}

void loop() {
    objectCount++;
    handleModbus(objectCount);
    delay(10); 
}
