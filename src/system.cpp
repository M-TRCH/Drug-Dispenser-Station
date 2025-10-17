#include "system.h"

HardwareSerial Serial3(RS485_RX3_PIN, RS485_TX3_PIN);
HardwareSerial Serial232(RS232_RX1_PIN, RS232_TX1_PIN);
RS485Class rs485(Serial3, RS485_DE_PIN, -1, -1);  // แก้ไข rs485 object ให้ใช้ DE pin ที่ถูกต้อง

void systemInit(void)
{
    Serial.begin(115200);
    Serial.println("[System] Initialization...");

    // แสดงข้อมูล RS485 pins
    Serial.print("[System] RS485 RX Pin: ");
    Serial.println(RS485_RX3_PIN);
    Serial.print("[System] RS485 TX Pin: ");
    Serial.println(RS485_TX3_PIN);
    Serial.print("[System] RS485 DE Pin: ");
    Serial.println(RS485_DE_PIN);

    // RS485 DE pin
    pinMode(RS485_DE_PIN, OUTPUT);
    digitalWrite(RS485_DE_PIN, LOW); // RX mode
    Serial.println("[System] RS485 DE pin set to RX mode (LOW)");

    // RS485
    Serial3.begin(MODBUS_BAUD);  // ใช้ MODBUS_BAUD แทน RS485_BAUDRATE
    Serial3.setTimeout(1000);
    Serial.print("[System] RS485 Serial3 started at ");
    Serial.print(MODBUS_BAUD);
    Serial.println(" baud");

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
