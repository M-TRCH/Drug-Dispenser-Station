#include "system.h"

void systemInit(void)
{
    // ตั้งค่าขา LED
    pinMode(LED_STATUS_PIN, OUTPUT);
    digitalWrite(LED_STATUS_PIN, HIGH);  

    // เริ่มต้น Serial USB debug
    Serial.begin(SERIAL_BAUDRATE);
    Serial.setTimeout(RS232_TIMEOUT);

    Serial.println("[System] Initialized");
}

void setLEDBuiltIn(bool state, int delay_time)
{
    digitalWrite(LED_STATUS_PIN, state ? LOW : HIGH); // Active LOW
    if (delay_time > 0) delay(delay_time);
}
