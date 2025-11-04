#ifndef SYSTEM_H
#define SYSTEM_H

#include <Arduino.h>
#include <HardwareSerial.h>
#include <ArduinoRS485.h>

// Pin definitions for the STM32G431CBU6 board == 20 PINS edited
#define LED_STATUS_PIN  PC13
#define SW_CALC_PIN     PA0
#define SW_START_PIN    PA1
#define PWM_A_PIN       PB0
#define PWM_B_PIN       PB1
#define PWM_C_PIN       PB13
#define SCK_PIN         PA5
#define MISO_PIN        PA6 
#define MOSI_PIN        PA7   
#define CS_ENC_PIN      PB12
#define SENSE_A_PIN     PA2
#define SENSE_C_PIN     PA3
#define SEN_1_PIN       PB9
#define SEN_2_PIN       PA15
#define RS485_RX3_PIN   PB11    // RS485 RX3
#define RS485_TX3_PIN   PB10    // RS485 TX3
#define RS232_RX1_PIN   PC4     // RS232 RX1
#define RS232_TX1_PIN   PB7     // RS232 TX1
#define SYS_RX1_PIN     PA10    // System RX1
#define SYS_TX1_PIN     PA9     // System TX1
#define RS485_DUMMY_PIN PA4     // RS485 Dummy pin
#define SYS_SERIAL_TIMEOUT      1000U   // SYS Serial Timeout (ms)
#define RS485_TIMEOUT           500U    // RS485 communication timeout (ms)
#define RS232_TIMEOUT           500U    // RS232 communication timeout (ms)

// System constants
#define SYSTEM_BAUDRATE          9600U      // System serial baudrate
#define RS485_BAUDRATE           9600U      // RS485 baudrate
#define RS232_BAUDRATE           9600U      // RS232 baudrate
#define ANALOG_READ_RESOLUTION   12U        // ADC resolution (0-4095)
#define MOTOR_PWM_FREQUENCY      20000U     // PWM frequency: 20kHz 
#define MOTOR_PWM_RESOLUTION     12U        // PWM resolution: 12-bit (0-4095)

// Object definitions
extern HardwareSerial Serial;
extern HardwareSerial Serial3;   
extern RS485Class rs485;

// Global system status variables
extern bool systemCalibrated;

// Macros definitions
#define SW_START_PRESSING  (digitalRead(SW_START_PIN) == LOW)
#define SW_CALC_PRESSING   (digitalRead(SW_CALC_PIN) == LOW)
#define SENSOR_1_ACTIVE    (digitalRead(SEN_1_PIN) == LOW)
#define SENSOR_2_ACTIVE    (digitalRead(SEN_2_PIN) == LOW)

/* @brief System initialization function
 * 
 * This function initializes the system by setting up serial communication,
 * configuring pin modes, and setting ADC and PWM resolutions.
 */
void systemInit();

/* @brief Test Serial1 communication
 * 
 * This function tests the Serial1 communication by looking for a '#' character
 * and printing numbers up to the parsed integer value.
 */
void testSerial1();

/* @brief Test Serial3 communication
 * 
 * This function tests the Serial3 communication by looking for a '#' character
 * and printing numbers up to the parsed integer value.
 */
void testSerial3();

/* @brief Perform system calibration
 * 
 * This function performs a complete system calibration including sensor checks
 * and motor testing. Sets systemCalibrated flag upon completion.
 */
void performSystemCalibration();

#endif

