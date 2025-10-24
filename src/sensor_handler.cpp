#include "sensor_handler.h"
#include "motor_control.h"
#include "system.h"

// Local variables
volatile unsigned long sensorTriggerCount = 0;
static unsigned long lastTriggerTime = 0;

void sensorInit() {
    // Configure sensor pin with pull-up resistor
    pinMode(SEN_1_PIN, INPUT_PULLUP);
    
    // Attach interrupt for falling edge detection
    attachInterrupt(digitalPinToInterrupt(SEN_1_PIN), sensorISR, FALLING);
    
    // Reset counters
    sensorTriggerCount = 0;
    lastTriggerTime = 0;
    
    Serial.println("[Sensor] FS-N11N optical sensor initialized");
}

void sensorISR() {
    unsigned long currentTime = micros();
    
    // Debounce: check minimum interval between triggers
    if (currentTime - lastTriggerTime > SENSOR_MIN_INTERVAL_US) {
        lastTriggerTime = currentTime;
        sensorTriggerCount++;
        
        // Count objects on every second trigger (for beam-break sensors)
        if (sensorTriggerCount % 2 == 0) {
            objectCount++;
        }
    }
}

void handleSensorLogic() {
    // Reserved for future sensor processing logic
    // Currently no additional processing needed
}
