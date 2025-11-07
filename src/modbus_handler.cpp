#include "modbus_handler.h"

ModbusRTUServerClass rtu;

// Shared variables
bool isEnhancedHomingActive = false;  // Enhanced Homing state

void modbusInit() 
{
    // Initialize Modbus RTU server
    if (!rtu.begin(rs485, MODBUS_SLAVE_ID, RS485_BAUDRATE, SERIAL_8N1)) {
        Serial.println("[Modbus] ERROR: Failed to start RTU Server!");
        return;
    }

    // Configure holding registers
    rtu.configureHoldingRegisters(0, MODBUS_REGISTER_COUNT);

    // Initialize register values
    rtu.holdingRegisterWrite(ADDR_REG_SPEED, MOTOR_PWM_DEFAULT);  // Default speed
    rtu.holdingRegisterWrite(ADDR_REG_DISP, 0);                  // No dispense command
    rtu.holdingRegisterWrite(ADDR_REG_HOME, HOME_CMD_NONE);       // No home command

    // Initialize status registers
    rtu.holdingRegisterWrite(ADDR_REG_STATUS, 0);                // Clear status
    rtu.holdingRegisterWrite(ADDR_REG_POSITION, 0);              // Position = 0
    rtu.holdingRegisterWrite(ADDR_REG_ERROR, ERR_NONE);          // No error

    Serial.printf("[Modbus] RTU Server started (Slave ID: %d)\n", MODBUS_SLAVE_ID);
}

void modbusHandler()
{
    // Poll Modbus communication
    rtu.poll();
    
    // Check for home command
    processHomeCommand(rtu.holdingRegisterRead(ADDR_REG_HOME));
    
    // Check for dispense command
    processDispenseCommand(rtu.holdingRegisterRead(ADDR_REG_DISP));
    
    // ตรวจสอบ Enhanced Homing (HOME + DISPENSE)
    updateHomingProcess();
    
    // Update status registers
    updateStatusRegisters();
}

void processHomeCommand(int homeCmd)
{
    // ตรวจสอบคำสั่ง HOME
    if (homeCmd == HOME_CMD_FIND) 
    {
        Serial.println("[Modbus] HOME command received - Starting home sequence");
        
        // Clear the command register
        rtu.holdingRegisterWrite(ADDR_REG_HOME, HOME_CMD_NONE);
        
        // เริ่มการทำ HOME เพียงอย่างเดียว
        home_start(1, MOTOR_PWM_SLOW);
        
        Serial.println("[Modbus] Home operation started via Modbus");
    }
    else if (homeCmd == HOME_CMD_RETURN) 
    {
        Serial.println("[Modbus] HOME+DISPENSE command received - Starting home then dispense sequence");
        
        // Clear the command register
        rtu.holdingRegisterWrite(ADDR_REG_HOME, HOME_CMD_NONE);
        
        // เริ่มการทำ HOME ก่อน (เหมือนการกดปุ่ม SW_CALC)
        home_start(1, MOTOR_PWM_SLOW);
        
        // ตั้งค่า flag เพื่อให้ dispense หลังจาก home เสร็จ
        isEnhancedHomingActive = true;
        
        Serial.println("[Modbus] Home+Dispense sequence started via Modbus");
    }
    else if (homeCmd == HOME_CMD_SEEK) 
    {
        Serial.println("[Modbus] HOME SEEK command received - Seeking home sensor");
        
        // Clear the command register
        rtu.holdingRegisterWrite(ADDR_REG_HOME, HOME_CMD_NONE);
        
        // เริ่มการค้นหา HOME sensor
        home_seek_start(MOTOR_PWM_SLOW);
        
        Serial.println("[Modbus] Home seek operation started via Modbus");
    }
}

void processDispenseCommand(int dispenseRotations)
{
    // ตรวจสอบคำสั่ง DISPENSE (1-99 รอบ)
    if (dispenseRotations > 0 && dispenseRotations <= 99) 
    {
        Serial.printf("[Modbus] DISPENSE command received - %d rotations\n", dispenseRotations);
        
        // Clear the command register
        rtu.holdingRegisterWrite(ADDR_REG_DISP, 0);
        
        // อ่านความเร็วจาก register SPEED
        int motorSpeed = rtu.holdingRegisterRead(ADDR_REG_SPEED);
        
        // ตรวจสอบความเร็วให้อยู่ในช่วงที่ถูกต้อง
        if (motorSpeed < MOTOR_PWM_MIN || motorSpeed > MOTOR_PWM_MAX) {
            motorSpeed = MOTOR_PWM_DEFAULT;
            Serial.printf("[Modbus] Invalid speed, using default: %d\n", motorSpeed);
        }
        
        // เริ่มการจ่าย
        dispense_start(dispenseRotations, motorSpeed);
        
        Serial.printf("[Modbus] Dispense operation started: %d rotations at speed %d\n", 
                     dispenseRotations, motorSpeed);
    }
}

void updateStatusRegisters()
{
    // อัปเดต status register
    uint16_t status = 0;
    
    // ตรวจสอบสถานะต่างๆ
    if (flag_motorRunning) 
    {
        status |= STATUS_MOTOR_RUNNING;
    }
    
    if (dispenseActive) 
    {
        status |= STATUS_DISPENSE_ACTIVE;
    }
    
    if (homeActive) 
    {
        status |= STATUS_HOMING;
    }
    
    if (homeSeeking) 
    {
        status |= STATUS_HOMING;  // ใช้ flag เดียวกันสำหรับการ seek sensor
    }
    
    if (homeCompleted) 
    {
        status |= STATUS_HOME_FOUND;
        status |= STATUS_AT_HOME;
    }
    
    if (isEnhancedHomingActive) 
    {
        status |= STATUS_CALIBRATING;  // ใช้ bit นี้แสดงว่ากำลังทำ Enhanced Homing
    }
    
    // เขียนค่า status ลง register
    rtu.holdingRegisterWrite(ADDR_REG_STATUS, status);
    
    // อัปเดต position (scaled x100)
    rtu.holdingRegisterWrite(ADDR_REG_POSITION, rotationCounter * 100);
    
    // อัปเดต error code จากระบบ
    rtu.holdingRegisterWrite(ADDR_REG_ERROR, lastErrorCode);
}

void updateHomingProcess()
{
    // ตรวจสอบว่า Enhanced Homing (HOME + DISPENSE) เปิดใช้งานหรือไม่
    if (isEnhancedHomingActive) 
    {
        // ตรวจสอบว่า HOME เสร็จแล้วและไม่มี operation อื่นทำงานอยู่
        if (homeCompleted && !homeActive && !dispenseActive) 
        {
            Serial.println("[Modbus] Home completed, starting dispense sequence");
            
            // อ่านพารามิเตอร์สำหรับ dispense
            int motorSpeed = rtu.holdingRegisterRead(ADDR_REG_SPEED);
            
            // ตรวจสอบความเร็วให้อยู่ในช่วงที่ถูกต้อง
            if (motorSpeed < MOTOR_PWM_MIN || motorSpeed > MOTOR_PWM_MAX) 
            {
                motorSpeed = MOTOR_PWM_DEFAULT;
            }
            
            // เริ่ม dispense (ใช้ค่าเริ่มต้น 10 รอบ หรือจะอ่านจาก register อื่น)
            dispense_start(10, motorSpeed);
            
            // ปิด Enhanced Homing
            isEnhancedHomingActive = false;
            
            Serial.printf("[Modbus] Enhanced Homing completed - Dispense started: 10 rotations at speed %d\n", motorSpeed);
        }
    }
}
