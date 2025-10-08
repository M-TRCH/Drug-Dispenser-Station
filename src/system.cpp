#include "system.h"

//extern HardwareSerial Serial3(RS485_RX3_PIN, RS485_TX3_PIN);  // RX, TX
//extern HardwareSerial Serial232(RS232_RX1_PIN, RS232_TX1_PIN);  // RX, TX

void systemInit(void)
{
    // --- Serial 1 (USB or UART1) ---
    Serial.begin(SERIAL_BAUDRATE);
    Serial.setTimeout(SERIAL_TIMEOUT);

    // --- Serial3 (RS485) ---
    Serial3.begin(SERIAL3_BAUDRATE);
    Serial3.setTimeout(SERIAL_TIMEOUT);

    // --- Pin configuration ---
    pinMode(LED_STATUS_PIN, OUTPUT);
    pinMode(RS485_DE_PIN, OUTPUT);
    digitalWrite(RS485_DE_PIN, HIGH); // Enable RS485 transmit

    // --- Test Serial3 availability ---
    Serial.println("[System] Testing Serial3...");
    Serial3.println("[Echo Test] Serial3 ready!");

    delay(100);
    bool serial3_ok = false;
    while (Serial3.available()) {
        String msg = Serial3.readStringUntil('\n');
        if (msg.length() > 0) {
            serial3_ok = true;
            Serial.print("[System] Serial3 echo OK: ");
            Serial.println(msg);
            break;
        }
    }

    if (serial3_ok) {
        Serial.println("[System] Serial3 communication OK");
        digitalWrite(LED_STATUS_PIN, LOW); // LED ON
    } else {
        Serial.println("[System] Serial3 failed (check RX/TX wiring)");
        digitalWrite(LED_STATUS_PIN, HIGH); // LED OFF
    }
}

void setLEDBuiltIn(bool state, int delay_time)
{
    digitalWrite(LED_STATUS_PIN, state ? LOW : HIGH);
    if (delay_time > 0) delay(delay_time);
}
