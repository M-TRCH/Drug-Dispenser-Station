
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
#define SYS_RX1_PIN     PA9     // SYS RX1
#define SYS_TX1_PIN     PA10    // SYS TX1
#define RS485_DE_PIN    PA8

// System constants
#define SERIAL_BAUDRATE          9600U
#define SERIAL_TIMEOUT           500U
#define SERIAL_DECIMAL_PLACES    2
#define SERIAL3_BAUDRATE         9600U
#define SERIAL3_TIMEOUT          500U
#define SERIAL3_DECIMAL_PLACES   2
#define RS232_BAUDRATE           115200U
#define RS232_TIMEOUT            500U

#define ANALOG_READ_RESOLUTION   12U
#define MY_PWM_FREQUENCY         20000U
#define MY_PWM_RESOLUTION        12U

// Object definitions
extern HardwareSerial Serial;
extern HardwareSerial Serial3;   
extern RS485Class rs485;

// Macros definitions
#define SW_START_PRESSING  (digitalRead(SW_START_PIN) == LOW)
#define SW_CALC_PRESSING   (digitalRead(SW_CALC_PIN) == LOW)
#define SENSOR_1_ACTIVE    (digitalRead(SEN_1_PIN) == LOW)
#define SENSOR_2_ACTIVE    (digitalRead(SEN_2_PIN) == LOW)

// Communication settings
#define DEBUG_BAUD      9600
#define MODBUS_BAUD     9600

/* @brief System initialization function
 * 
 * This function initializes the system by setting up serial communication,
 * configuring pin modes, and setting ADC and PWM resolutions.
 */
void systemInit();

/* @brief Set the state of built-in LEDs
 * 
 * This function controls the state of the built-in LEDs for run, calibration,
 * and error indicators. It also includes an optional delay.
 * 
 * @param run  Set to true to turn on the run LED, false to turn it off.
 * @param cal  Set to true to turn on the calibration LED, false to turn it off.
 * @param err  Set to true to turn on the error LED, false to turn it off.
 * @param delay_time  Optional delay time in milliseconds after setting the LEDs.
 */
void setLEDBuiltIn(bool state, int delay_time = 0);
#endif
