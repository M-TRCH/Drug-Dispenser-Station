#include "sensor_handler.h"
#include "motor_control.h"

volatile bool sensorTriggered = false;
unsigned long lastTriggerTime = 0;

void sensorInit() {
  pinMode(SEN_1_PIN, INPUT_PULLUP); // NPN → ใช้ pull-up
  attachInterrupt(digitalPinToInterrupt(SEN_1_PIN), sensorISR, FALLING);
  Serial3.println("[Sensor] FS-N11N initialized (NPN, falling edge)");
}

void sensorISR() {
  unsigned long now = millis();
  if (now - lastTriggerTime > SENSOR_DEBOUNCE_TIME_MS) {
    lastTriggerTime = now;
    sensorTriggered = true;
  }
}

void handleSensorLogic() {
  if (sensorTriggered && motorRunning && countingMode) {
    sensorTriggered = false;
    objectCount++;
    Serial3.printf("[Sensor] Count = %lu\n", objectCount);
  }
}
