#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <Arduino.h>

// === PWM Configuration ===
#define ANALOG_READ_RESOLUTION   12U
#define PWM_FREQUENCY            20000U   // 20kHz 
#define PWM_RESOLUTION           12U      // 12-bit PWM (0–4095)

//== velocity == // Velocity control for the motor == PWM speed control(0 - 4095 12-bit )
#define MOTOR_PWM_MIN        1000
#define MOTOR_PWM_MAX        4000
#define MOTOR_PWM_DEFAULT    3000

// === Start/stop delay for motor and dispenser ===
#define MOTOR_START_DELAY_MS  200
#define MOTOR_STOP_DELAY_MS   200

// === Counter === // Object counter for the dispenser == กำหนดจำนวนวัตถุที่ต้องการจ่าย ==
#define DISPENSE_TARGET_DEFAULT  10
#define SENSOR_DEBOUNCE_TIME_MS  30

// === Dispenser Activate === // Control for activating the dispenser  ==
#define DISPENSER_PIN          PB5      // ตัวอย่าง pin output
#define DISPENSER_ACTIVE_LEVEL HIGH     // ระดับ logic ที่ทำให้เปิดใช้งาน

// === Extern Variables  ===
extern bool motorRunning;
extern bool countingMode;
extern volatile unsigned long objectCount;
extern unsigned long targetCount;

// === Function Prototypes ===
void motorControlInit();

void startMotor(int pwmSpeed, bool forward);
void stopMotor(bool error = false);
void setMotorSpeed(int pwmSpeed);

void dispenserOn();
void dispenserOff();

void resetCounter();
bool isTargetReached();

void sensorTriggerISR();


#endif

//#ifndef MOTOR_CONTROL_H
//#define MOTOR_CONTROL_H
//
//#include <Arduino.h>
//#include "system.h"
//
//// ===================== PWM Settings =====================
//#undef PWM_FREQUENCY
//#undef PWM_RESOLUTION
//
//#define PWM_FREQUENCY   20000U   // 20 kHz
//#define PWM_RESOLUTION  12U      // 12-bit (0–4095)
//
//// ===================== Motor Control Variables =====================
//extern unsigned long targetCount;   // จำนวนรอบที่ตั้งไว้
//extern unsigned long currentCount;  // จำนวนรอบปัจจุบัน
//extern bool motorRunning;           // สถานะมอเตอร์ทำงานอยู่ไหม
//
//// ===================== Function Prototypes =====================
//void motorInit();
//void motorStart();
//void motorStop();
//void motorUpdate();
//void handleSensorTrigger();

