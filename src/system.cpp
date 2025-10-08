#include "system.h"

HardwareSerial Serial3(RS485_RX3_PIN, RS485_TX3_PIN);
HardwareSerial Serial232(RS232_RX1_PIN, RS232_TX1_PIN);

void systemInit(void)
{
    Serial.begin(115200);
    Serial.println("[System] Initialization...");

    // RS485
    Serial3.begin(RS485_BAUDRATE);
    Serial3.setTimeout(1000);

    // RS232
    Serial232.begin(RS232_BAUDRATE);
    Serial232.setTimeout(1000);

    // LED + RS485 Control Pin
    pinMode(LED_STATUS_PIN, OUTPUT);
    digitalWrite(LED_STATUS_PIN, HIGH); // LED OFF (Active LOW)

    Serial.println("[System] Init complete.");
    setLEDBuiltIn(true, 200);
    setLEDBuiltIn(false, 0);
}

void setLEDBuiltIn(bool state, int delay_time)
{
    // Active LOW LED
    digitalWrite(LED_STATUS_PIN, state ? LOW : HIGH);
    if (delay_time > 0) delay(delay_time);
}
