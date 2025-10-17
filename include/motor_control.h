#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <Arduino.h>

//== velocity == // Velocity control for the motor == PWM speed control(0 - 4095 12-bit )
#define MOTOR_PWM_MIN        1000
#define MOTOR_PWM_MAX        4000
#define MOTOR_PWM_DEFAULT    3000

// === Velocity === // Velocity control for the motor (Modbus Holding Register)
#define MOTOR_VELOCITY_MIN     500      // ความเร็วต่ำสุด
#define MOTOR_VELOCITY_MAX     4000     // ความเร็วสูงสุด  
#define MOTOR_VELOCITY_DEFAULT 3000     
#define MOTOR_HOMING_SPEED     1500     // ความเร็วสำหรับ homing และการนับรอบ (ใช้เดียวกัน)     

// === Counter === // Object counter for the dispenser (Modbus Holding Register)
#define DISPENSE_TARGET_DEFAULT  10
#define DISPENSE_TARGET_MIN      1
#define DISPENSE_TARGET_MAX      9999
#define SENSOR_MIN_INTERVAL_US   500    // ห้ามนับเร็วกว่า 500µs    

// === Dispenser Activate === // Control for activating the dispenser (Modbus Coil)
#define DISPENSER_PIN          PB5      // ตัวอย่าง pin output
#define DISPENSER_ACTIVE_LEVEL HIGH     // ระดับ logic ที่ทำให้เปิดใช้งาน

// === Button Configuration ===
#define BUTTON_DEBOUNCE_TIME_MS  20     
#define BUTTON_CHECK_INTERVAL_MS 10 
#define SENSOR_MIN_INTERVAL_US   500    

// === Motor Control Timing ===
#define MOTOR_START_DELAY_MS  100       
#define MOTOR_STOP_DELAY_MS   100

// === Extern Variables  ===
extern bool motorRunning;
extern bool countingMode;
extern volatile unsigned long objectCount;
extern unsigned long targetCount;
// objectCount จะนับ “รอบจริง” แล้ว (ทุก 2 สัญญาณ FS-N11N)
extern volatile unsigned long objectCount;

// === Modbus Variables ===
// Holding Registers (16-bit values)
extern unsigned int motorVelocity;       // Address 0: ความเร็วมอเตอร์ (500-4000)
extern unsigned int dispenseTarget;      // Address 1: จำนวนเป้าหมาย (1-9999)
extern unsigned int currentCount;        // Address 2: จำนวนปัจจุบัน (read-only)
extern unsigned int systemStatus;        // Address 3: สถานะระบบ (read-only)

// Coils (1-bit values) 
extern bool dispenserActivate;           // Address 0: เปิด/ปิด การจ่าย
extern bool systemStart;                 // Address 1: เริ่มการทำงาน
extern bool systemStop;                  // Address 2: หยุดการทำงาน  
extern bool systemHome;                  // Address 3: เริ่ม homing

// === Function Prototypes ===
void motorControlInit();

void startMotor(int pwmSpeed, bool forward);
void stopMotor(bool error = false);
void setMotorSpeed(int pwmSpeed);

void dispenserOn();
void dispenserOff();

void resetCounter();
bool isTargetReached();

// === System Control Functions ===
void startOperation();           // เริ่มการทำงาน
// void handleSerialCommands();     // ย้ายไปอยู่ใน main.cpp แล้ว
void printSystemStatus();       // แสดงสถานะระบบ
void handleSystemButtons();     // จัดการปุ่มกด
void updateSystemState();       // อัปเดต state machine
void syncModbusVariables();     // ซิงค์ตัวแปร Modbus

// void sensorTriggerISR();  // ย้ายไปอยู่ใน sensor_handler.h แล้ว

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

