#include "system.h"

HardwareSerial Serial3(RS485_RX3_PIN, RS485_TX3_PIN);
RS485Class rs485(Serial3, RS485_DE_PIN, -1, -1); // ใช้ DE=PA8 (RE รวมกับ DE)

void systemInit(void)
{
    Serial.begin(115200);
    Serial.println("[System] Init...");

    // ตั้งค่า RS485 DE pin
    pinMode(RS485_DE_PIN, OUTPUT);
    digitalWrite(RS485_DE_PIN, LOW); // เริ่มต้นในโหมดรับ

    // RS485 Serial
    Serial3.begin(MODBUS_BAUD);  // 9600,8,N,1
    Serial3.setTimeout(1000);
    Serial.println("[System] RS485 Serial3 started @9600 8N1");

    // LED สถานะ
    pinMode(LED_STATUS_PIN, OUTPUT);
    digitalWrite(LED_STATUS_PIN, HIGH); // ปิด LED (Active LOW)
    Serial.println("[System] LED ready");

    setLEDBuiltIn(true, 100);
    setLEDBuiltIn(false, 0);

    Serial.println("[System] Init complete.");
}

void setLEDBuiltIn(bool state, int delay_time)
{
    digitalWrite(LED_STATUS_PIN, state ? LOW : HIGH);
    if (delay_time > 0) delay(delay_time);
}
