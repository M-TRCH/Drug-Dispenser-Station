#include "system.h"
#include "motor_control.h"
#include "modbus_handler.h"
#include "sensor_handler.h"

void setup() {
    systemInit();
    motorControlInit();
    sensorInit();
    setupModbus();

    Serial.println("[Main] System Ready!");
}

void loop() {
    handleModbus();
    handleSensorLogic();
    delay(20);  // ป้องกัน Modbus timeout
}
