
#include "system.h"

HardwareSerial Serial(SYS_RX1_PIN, SYS_TX1_PIN);           // RX, TX pins for Serial2
HardwareSerial Serial3(RS485_RX3_PIN, RS485_TX3_PIN);         // RX3, TX3 pins for Serial3 (debugging)
RS485Class rs485(Serial3, PA7, SYS_TX1_PIN, SYS_RX1_PIN);

void systemInit() 
{
    // Serial for RS485 communication
    Serial.begin(MODBUS_BAUD, SERIAL_8N1);

    // Serial3 for debugging
    Serial3.begin(SERIAL3_BAUDRATE);
    Serial3.setTimeout(SERIAL3_TIMEOUT);

    // Initialize pins
    pinMode(LED_STATUS_PIN, OUTPUT); 
    pinMode(SW_CALC_PIN, INPUT);
    pinMode(SW_START_PIN, INPUT);
    pinMode(SEN_1_PIN, INPUT);
    pinMode(SEN_2_PIN, INPUT);

    analogReadResolution(ANALOG_READ_RESOLUTION); // Set ADC resolution to 12 bits for current sensing

    analogWriteFrequency(MY_PWM_FREQUENCY);
    analogWriteResolution(MY_PWM_RESOLUTION);  // Set PWM resolution to 12 bits for SVPWM
}

void setLEDBuiltIn(bool run, bool cal, bool err, int delay_time)
{
    digitalWrite(LED_STATUS_PIN, run || cal || err ? LOW : HIGH); // Active LOW for built-in LED
    delay(abs(delay_time));
}