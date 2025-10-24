#include "system.h"

HardwareSerial Serial3(RS485_RX3_PIN, RS485_TX3_PIN);
RS485Class rs485(Serial3, RS485_DE_PIN, -1, -1); // ใช้ DE=PA8 (RE รวมกับ DE)

void systemInit(void)
{
    // Initialize debug serial communication
    Serial.begin(DEBUG_BAUDRATE);
    Serial.println("[System] Initializing...");

    // Configure RS485 DE (Driver Enable) pin
    pinMode(RS485_DE_PIN, OUTPUT);
    digitalWrite(RS485_DE_PIN, LOW); // Start in receive mode

    // Initialize RS485 Serial communication
    Serial3.begin(MODBUS_BAUDRATE);
    Serial3.setTimeout(COMMUNICATION_TIMEOUT);
    Serial.printf("[System] RS485 Serial3 started @%u 8N1\n", MODBUS_BAUDRATE);

    // Configure status LED
    pinMode(LED_STATUS_PIN, OUTPUT);
    digitalWrite(LED_STATUS_PIN, HIGH); // Turn off LED (Active LOW)
    Serial.println("[System] Status LED configured");

    // Configure ADC and PWM resolutions
    analogReadResolution(ANALOG_READ_RESOLUTION);
    analogWriteResolution(MOTOR_PWM_RESOLUTION);
    analogWriteFrequency(MOTOR_PWM_FREQUENCY);
    Serial.println("[System] ADC/PWM configured");

    // LED startup sequence
    setLEDBuiltIn(true, 100);
    setLEDBuiltIn(false, 0);

    Serial.println("[System] Initialization complete");
}

void setLEDBuiltIn(bool state, int delay_time)
{
    digitalWrite(LED_STATUS_PIN, state ? LOW : HIGH);
    if (delay_time > 0) delay(delay_time);
}
