
#ifndef SYSTEM_H
#define SYSTEM_H

#include <Arduino.h>
#include <HardwareSerial.h>
#include <ArduinoRS485.h>

// Pin definitions for the STM32G431CBU6 board
#define LED_RUN_PIN     PC10
#define LED_CAL_PIN     PC11
#define LED_ERR_PIN     PC13
#define SW_STOP_PIN     PA0
#define SW_START_PIN    PA1
#define PWM_A_PIN       PB0
#define PWM_B_PIN       PB1
#define PWM_C_PIN       PB13
#define MISO_PIN        PA6 
#define MOSI_PIN        PA7   
#define SCK_PIN         PA5
#define CS_ENC_PIN      PB12
#define ADC_IA_PIN      PA2
#define ADC_IC_PIN      PA3
#define ADC_VDC_PIN     PA4
#define SEN_1_PIN       PA15
#define SEN_2_PIN       PB9
#define RX_PIN          PB11  // RX pin for Serial2
#define TX_PIN          PB10  // TX pin for Serial2
#define RX3_PIN         PA10
#define TX3_PIN         PA9

// (1) System constants
#define SERIAL3_BAUDRATE         2000000U
#define SERIAL3_TIMEOUT          500U
#define SERIAL3_DECIMAL_PLACES   2
#define ANALOG_READ_RESOLUTION   12U
#define MY_PWM_FREQUENCY         20000U
#define MY_PWM_RESOLUTION        12U

// (2) Object definitions
extern HardwareSerial Serial2;   // RX, TX pins for Serial2
extern HardwareSerial Serial3;   // RX3, TX3 pins for Serial3 (debugging)
extern RS485Class rs485;

// (3) Macros definitions
#define SW_START_PRESSING  (digitalRead(SW_START_PIN) == LOW)
#define SW_STOP_PRESSING   (digitalRead(SW_STOP_PIN) == LOW)
#define SENSOR_1_ACTIVE    (digitalRead(SEN_1_PIN) == LOW)
#define SENSOR_2_ACTIVE    (digitalRead(SEN_2_PIN) == LOW)

// (5) Communication settings
#define DEBUG_Baud      9600
#define MODBUS_Baud     9600

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
void setLEDBuiltIn(bool run=false, bool cal=false, bool err=false, int delay_time=0);

#endif
