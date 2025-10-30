# Drug Dispenser Station - Modbus Control Guide

## 📡 การควบคุมผ่าน Modbus RTU

ระบบ Drug Dispenser Station สามารถควบคุมผ่าน Modbus RTU ได้แบบครบถ้วน รองรับการสั่งงานและการอ่านสถานะต่างๆ

### 🔧 การตั้งค่า Modbus

```cpp
Slave ID: 55
Baud Rate: 9600
Data Bits: 8
Parity: None  
Stop Bits: 1
Function Code: 0x03 (Read Holding Registers), 0x06 (Write Single Register)
```

### 📝 Register Map

## Control Registers (Write) - สำหรับสั่งงาน

| Address | Name | Description | Range | Unit |
|---------|------|-------------|-------|------|
| 10 | REG_SPEED | ความเร็วมอเตอร์ | 0-4095 | PWM Value |
| 11 | REG_DISP | คำสั่งจ่ายยา | 1-99 | Rotations |
| 12 | REG_CMD | คำสั่งระบบ | 0-3 | Command Code |
| 13 | REG_HOME | คำสั่งหา Home | 0-1 | 1=Execute |
| 14 | REG_CALIBRATE | คำสั่งคาลิเบรต | 0-1 | 1=Execute |
| 15 | REG_RETURN_HOME | คำสั่งกลับ Home | 0-1 | 1=Execute |
| 16 | REG_RESET | คำสั่งรีเซ็ต | 0-1 | 1=Execute |

## Status Registers (Read) - สำหรับอ่านสถานะ

| Address | Name | Description | Range | Unit |
|---------|------|-------------|-------|------|
| 20 | REG_STATUS | สถานะระบบ (bit flags) | 0-255 | Bit Flags |
| 21 | REG_POSITION | ตำแหน่งปัจจุบัน | 0-9999 | x100 (scaled) |
| 22 | REG_ROTATION_COUNT | ตัวนับรอบ | 0-65535 | Count |
| 23 | REG_ERROR_CODE | รหัสข้อผิดพลาด | 0-5 | Error Code |
| 24 | REG_MOTOR_STATUS | สถานะมอเตอร์ | 0-65535 | Status Word |
| 25 | REG_HOME_STATUS | สถานะ Home Position | 0-1 | 0=Not Found, 1=Found |
| 26 | REG_CALIBRATED | สถานะการคาลิเบรต | 0-1 | 0=Not Cal, 1=Calibrated |
| 27 | REG_TARGET_ROTATIONS | เป้าหมายรอบปัจจุบัน | 0-99 | Rotations |

### 🎯 System Command Codes (Register 12)

| Code | Command | Description |
|------|---------|-------------|
| 0 | CMD_NONE | ไม่มีคำสั่ง |
| 1 | CMD_STOP | หยุดฉุกเฉิน |
| 2 | CMD_TEST_MOTOR | ทดสอบมอเตอร์ |
| 3 | CMD_GET_STATUS | อัปเดตสถานะ |

### 🚩 Status Bit Flags (Register 20)

| Bit | Flag | Description |
|-----|------|-------------|
| 0 | MOTOR_RUNNING | มอเตอร์กำลังทำงาน |
| 1 | DISPENSE_ACTIVE | กำลังจ่ายยา |
| 2 | HOME_FOUND | พบตำแหน่ง Home แล้ว |
| 3 | CALIBRATED | ระบบคาลิเบรตแล้ว |
| 4 | ERROR | มีข้อผิดพลาด |
| 5 | AT_HOME | อยู่ที่ตำแหน่ง Home |
| 6 | DECELERATING | มอเตอร์กำลังลดความเร็ว |

### ❌ Error Codes (Register 23)

| Code | Error | Description |
|------|-------|-------------|
| 0 | ERR_NONE | ไม่มีข้อผิดพลาด |
| 1 | ERR_HOME_NOT_FOUND | หาตำแหน่ง Home ไม่เจอ |
| 2 | ERR_NOT_CALIBRATED | ระบบยังไม่ได้คาลิเบรต |
| 3 | ERR_INVALID_ROTATION | จำนวนรอบไม่ถูกต้อง |
| 4 | ERR_MOTOR_TIMEOUT | มอเตอร์ timeout |
| 5 | ERR_POSITION_ERROR | ข้อผิดพลาดตำแหน่ง |

## 🚀 ตัวอย่างการใช้งาน

### 1. การเริ่มต้นระบบ (Initialization)

```python
# 1. อ่านสถานะระบบ
status = read_holding_register(20)  # REG_STATUS
calibrated = read_holding_register(26)  # REG_CALIBRATED

# 2. ถ้ายังไม่ได้คาลิเบรต ให้คาลิเบรต
if not (calibrated & 1):
    write_single_register(14, 1)  # REG_CALIBRATE = 1
    time.sleep(5)  # รอให้คาลิเบรตเสร็จ
    
# 3. ตรวจสอบสถานะหลังคาลิเบรต
calibrated = read_holding_register(26)
if calibrated & 1:
    print("System ready!")
```

### 2. การจ่ายยา (Dispensing)

```python
# ตั้งค่าความเร็วและจำนวนรอบ
write_single_register(10, 3000)  # REG_SPEED = 3000 PWM
write_single_register(11, 5)     # REG_DISP = 5 rotations

# รอจนกว่าจะจ่ายเสร็จ
while True:
    status = read_holding_register(20)  # REG_STATUS
    if not (status & 0x02):  # DISPENSE_ACTIVE bit
        break
    time.sleep(0.1)

print("Dispensing completed!")
```

### 3. การตรวจสอบตำแหน่ง (Position Monitoring)

```python
# อ่านตำแหน่งปัจจุบัน
position_scaled = read_holding_register(21)  # REG_POSITION
position = position_scaled / 100.0  # แปลงกลับเป็นทศนิยม

# อ่านตัวนับรอบ
rotation_count = read_holding_register(22)  # REG_ROTATION_COUNT

print(f"Position: {position:.2f} rotations")
print(f"Total rotations: {rotation_count}")
```

### 4. การหยุดฉุกเฉิน (Emergency Stop)

```python
# หยุดฉุกเฉิน
write_single_register(12, 1)  # REG_CMD = CMD_STOP

# ตรวจสอบว่าหยุดแล้ว
status = read_holding_register(20)
if not (status & 0x01):  # MOTOR_RUNNING bit
    print("Motor stopped successfully")
```

### 5. การกลับไปตำแหน่ง Home

```python
# กลับไป Home position
write_single_register(15, 1)  # REG_RETURN_HOME = 1

# รอจนกว่าจะถึง Home
while True:
    status = read_holding_register(20)
    if status & 0x20:  # AT_HOME bit
        break
    time.sleep(0.1)

print("Returned to home position")
```

## 🔍 การตรวจสอบข้อผิดพลาด (Error Handling)

```python
def check_system_error():
    error_code = read_holding_register(23)  # REG_ERROR_CODE
    
    if error_code == 1:
        print("ERROR: Cannot find home position")
        return False
    elif error_code == 2:
        print("ERROR: System not calibrated")
        return False
    elif error_code == 3:
        print("ERROR: Invalid rotation count")
        return False
    elif error_code == 4:
        print("ERROR: Motor timeout")
        return False
    elif error_code == 5:
        print("ERROR: Position accuracy error")
        return False
    
    return True  # No error
```

## 📊 การอ่านสถานะครบถ้วน (Complete Status Reading)

```python
def read_complete_status():
    # อ่านทุก status registers
    status_flags = read_holding_register(20)
    position = read_holding_register(21) / 100.0
    rotation_count = read_holding_register(22)
    error_code = read_holding_register(23)
    motor_status = read_holding_register(24)
    home_status = read_holding_register(25)
    calibrated = read_holding_register(26)
    target_rotations = read_holding_register(27)
    
    # แสดงสถานะ
    print("=== System Status ===")
    print(f"Motor Running: {bool(status_flags & 0x01)}")
    print(f"Dispense Active: {bool(status_flags & 0x02)}")
    print(f"Home Found: {bool(status_flags & 0x04)}")
    print(f"Calibrated: {bool(status_flags & 0x08)}")
    print(f"Error: {bool(status_flags & 0x10)}")
    print(f"At Home: {bool(status_flags & 0x20)}")
    print(f"Position: {position:.2f} rotations")
    print(f"Error Code: {error_code}")
    print(f"Target: {target_rotations} rotations")
```

## 🔌 การเชื่อมต่อ Hardware

### RS485 Connection
```
STM32G431CBU6 Pins:
- RS485_TX3: PB10
- RS485_RX3: PB11  
- RS485_DUMMY: PA4 (Direction control)

Modbus Settings:
- Slave ID: 55
- Baud Rate: 9600
- 8N1 (8 data bits, No parity, 1 stop bit)
```

## ⚠️ ข้อควรระวัง

1. **ต้องคาลิเบรตก่อนใช้งาน** - ระบบจะไม่ยอมให้จ่ายยาถ้ายังไม่ได้คาลิเบรต
2. **ตรวจสอบ Error Code** - ควรตรวจสอบ error code หลังจากสั่งงานทุกครั้ง
3. **รอให้คำสั่งเสร็จ** - ควรรอให้คำสั่งเสร็จก่อนสั่งคำสั่งถัดไป
4. **Monitor Status Flags** - ใช้ status flags เพื่อติดตามสถานะการทำงาน
5. **Position Scaling** - ตำแหน่งใน register 21 ถูก scale x100 ต้องหารด้วย 100

---
*อัปเดตล่าสุด: ตุลาคม 2025*