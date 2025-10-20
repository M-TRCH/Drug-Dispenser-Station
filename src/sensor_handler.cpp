#include "sensor_handler.h"
#include "motor_control.h"
#include "system.h"

volatile unsigned long sensorTriggerCount = 0;
unsigned long lastTriggerTime = 0;

void sensorInit() {
    pinMode(SEN_1_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(SEN_1_PIN), sensorISR, FALLING);
    Serial.println("[Sensor] FS-N11N Initialized");
}

void sensorISR() {
    unsigned long now = micros();
    if (now - lastTriggerTime > SENSOR_MIN_INTERVAL_US) {
        lastTriggerTime = now;
        sensorTriggerCount++;
        if (sensorTriggerCount % 2 == 0) objectCount++;
    }
}

void handleSensorLogic() {}
