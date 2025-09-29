
#include "system.h"

HardwareSerial Serial2(RX_PIN, TX_PIN); // RX, TX pins for Serial2
RS485Class rs485(Serial2, PA7, TX_PIN, RX_PIN);

void systemInit() 
{
    // Serial2 for RS485 communication
    Serial2.begin(MODBUS_BAUD, SERIAL_8N1);

    // Initialize pins
    pinMode(LED_RUN_PIN, OUTPUT);
    pinMode(LED_CAL_PIN, OUTPUT);
    pinMode(LED_ERR_PIN, OUTPUT); 
    pinMode(SW_STOP_PIN, INPUT);
    pinMode(SW_START_PIN, INPUT);
    pinMode(SEN_1_PIN, INPUT);
    pinMode(SEN_2_PIN, INPUT);

    analogReadResolution(ANALOG_READ_RESOLUTION); // Set ADC resolution to 12 bits for current sensing

    analogWriteFrequency(MY_PWM_FREQUENCY);
    analogWriteResolution(MY_PWM_RESOLUTION);  // Set PWM resolution to 12 bits for SVPWM
}

/* @brief       Set the built-in LEDs
 * @param run   State for the RUN LED
 * @param cal   State for the CAL LED
 * @param err   State for the ERR LED
 */
void setLEDBuiltIn(bool run, bool cal, bool err, int delay_time)
{
    digitalWrite(LED_RUN_PIN, run);
    digitalWrite(LED_CAL_PIN, cal);
    digitalWrite(LED_ERR_PIN, err);
    delay(abs(delay_time));
}