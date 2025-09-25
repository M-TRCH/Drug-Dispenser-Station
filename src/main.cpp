#include <Arduino.h>
#include "system.h"
#include <ArduinoModbus.h>

// ===== Modbus Config =====
#define SLAVE_ID     2
#define REG_RUN      0   // 0 = Stop, 1 = Run
#define REG_SPEED    1   // PWM Speed
#define REG_COUNT    2   // Object Count
#define REG_STATUS   3   // 0 = Idle, 1 = Running

// ==== Motor State ====
bool motorRunning     = false;
volatile unsigned long objectCount = 0; 

// ==== Motor Config ====
const int MOTOR_SPEED = 3000; // default speed 0–4095

// ===== ISR Prototype =====
void sensorISR();
void motorControlFromModbus();

void setup() {
  systemInit();

  // === Debug ผ่าน USB ===
  Serial.begin(115200);
  while (!Serial);

  // === Motor pin ===
  pinMode(PWM_A_PIN, OUTPUT);
  pinMode(PWM_B_PIN, OUTPUT);

  analogWriteResolution(MY_PWM_RESOLUTION);
  analogWriteFrequency(MY_PWM_FREQUENCY);

  // === Sensor ===
  pinMode(SEN_2_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(SEN_2_PIN), sensorISR, FALLING);

  // === Modbus RTU Slave ===
  Serial3.begin(9600, SERIAL_8N1);
  if (!ModbusRTUServer.begin(SLAVE_ID, Serial3)) {
    Serial.println("Failed to start Modbus RTU Server!");
    while (1);
  }

  // Register 0–7
  ModbusRTUServer.configureHoldingRegisters(0, 8);

  // ค่าเริ่มต้น
  ModbusRTUServer.holdingRegisterWrite(REG_RUN, 0);
  ModbusRTUServer.holdingRegisterWrite(REG_SPEED, MOTOR_SPEED);
  ModbusRTUServer.holdingRegisterWrite(REG_COUNT, 0);
  ModbusRTUServer.holdingRegisterWrite(REG_STATUS, 0);

  Serial.println("System Initialized. Modbus RTU ready.");
}

void loop() {
  // ===== Modbus =====
  ModbusRTUServer.poll();
  motorControlFromModbus();
}

// ===== Modbus Motor Control =====
void motorControlFromModbus() {
  int runCmd = ModbusRTUServer.holdingRegisterRead(REG_RUN);
  int pwmVal = ModbusRTUServer.holdingRegisterRead(REG_SPEED);

  if (runCmd == 1) {
    if (pwmVal == 0) pwmVal = MOTOR_SPEED;

    analogWrite(PWM_A_PIN, pwmVal);
    analogWrite(PWM_B_PIN, 0);

    if (!motorRunning) {
      motorRunning = true;
      ModbusRTUServer.holdingRegisterWrite(REG_STATUS, 1); // Running
      Serial.println("Motor RUN via Modbus");
    }
  } else {
    analogWrite(PWM_A_PIN, 0);
    analogWrite(PWM_B_PIN, 0);

    if (motorRunning) {
      motorRunning = false;
      ModbusRTUServer.holdingRegisterWrite(REG_STATUS, 0); // Idle
      Serial.println("Motor STOP via Modbus");
    }
  }

  // update object count
  ModbusRTUServer.holdingRegisterWrite(REG_COUNT, objectCount);
}

// ===== ISR =====
void sensorISR() {
  if (motorRunning) {
    objectCount++;
    Serial.print("Object Count = ");
    Serial.println(objectCount);
  }
}
