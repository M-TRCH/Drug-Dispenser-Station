#include "system.h"

HardwareSerial Serial3(RS485_RX3_PIN, RS485_TX3_PIN);
RS485Class rs485(Serial3, RS485_DUMMY_PIN, RS232_TX1_PIN, RS232_RX1_PIN); 

void systemInit(void)
{
    // Initialize system serial communication
    Serial.setRx(SYS_RX1_PIN);
    Serial.setTx(SYS_TX1_PIN);
    Serial.begin(SYSTEM_BAUDRATE);
    Serial.println("[System] Initializing...");

    // Initialize RS485 Serial communication
    Serial3.begin(RS485_BAUDRATE);
    Serial3.setTimeout(RS485_TIMEOUT);
    Serial.printf("[System] RS485 Serial3 started @%u 8N1\n", RS485_BAUDRATE);

    // Configure ADC and PWM resolutions
    analogReadResolution(ANALOG_READ_RESOLUTION);
    analogWriteResolution(MOTOR_PWM_RESOLUTION);
    analogWriteFrequency(MOTOR_PWM_FREQUENCY);
    Serial.println("[System] ADC/PWM configured");

    // Configure pin modes
    pinMode(SW_START_PIN, INPUT);
    pinMode(SW_CALC_PIN, INPUT);
    pinMode(SEN_1_PIN, INPUT);
    pinMode(SEN_2_PIN, INPUT);

    Serial.println("[System] Initialization complete");
}

void testSerial1()
{
    if (Serial.find('#'))
    {
        int num = Serial.parseInt();
        for (int i = 0; i < num; i++)
        {
            Serial.printf("print: %u\n", i);
            delay(500);
        }
    }
    else
    {
        Serial.println("No '#' found");
    }
}

void testSerial3()
{
    if (Serial3.find('#'))
    {
        int num = Serial3.parseInt();
        for (int i = 0; i < num; i++)
        {
            Serial3.printf("print: %u\n", i);
            delay(500);
        }
    }
    else
    {
        Serial3.println("No '#' found");
    }   
}