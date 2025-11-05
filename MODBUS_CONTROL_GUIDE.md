# ระบบ Modbus Control สำหรับ Drug Dispenser Station

## ภาพรวมระบบ

ระบบสามารถควบคุมผ่าน Modbus RTU ได้อย่างสมบูรณ์ โดยมีทั้งการควบคุม HOME และ DISPENSE พร้อมการตรวจสอบสถานะแบบ real-time

## การกำหนดค่า Modbus

```cpp
#define MODBUS_SLAVE_ID         55        // Modbus slave address
#define MODBUS_REGISTER_COUNT   50        // Number of holding registers
#define RS485_BAUDRATE          9600      // Baudrate: 9600, 8N1
```

## Modbus Register Map

### คำสั่งควบคุม (Command Registers)

| Register | Address | Function | Valid Values | Description |
|----------|---------|----------|--------------|-------------|
| SPEED | 10 | ตั้งความเร็วมอเตอร์ | 0-4095 | PWM speed value |
| DISP | 11 | คำสั่งจ่ายยา | 1-99 | จำนวนรอบที่ต้องการจ่าย |
| HOME | 12 | คำสั่ง HOME | 0,1,2 | 0=None, 1=Home only, 2=Home+Dispense |

### สถานะ (Status Registers - Read Only)

| Register | Address | Function | Description |
|----------|---------|----------|-------------|
| STATUS | 20 | สถานะระบบ | Bit flags แสดงสถานะการทำงาน |
| POSITION | 21 | ตำแหน่งปัจจุบัน | ตำแหน่งปัจจุบัน (scaled x100) |
| ERROR | 22 | รหัสข้อผิดพลาด | Error code (0=No error) |

## รายละเอียดคำสั่ง

### 1. HOME Commands (Register 12)
```
0 = HOME_CMD_NONE     // ไม่มีคำสั่ง
1 = HOME_CMD_FIND     // ทำ HOME เพียงอย่างเดียว (1 รอบ)
2 = HOME_CMD_RETURN   // ทำ HOME + จ่ายยาตามค่าที่กำหนด
```

### 2. DISPENSE Commands (Register 11)
```
0     = ไม่จ่าย
1-99  = จำนวนรอบที่ต้องการจ่าย
```

### 3. SPEED Setting (Register 10)
```
0-4095 = ค่า PWM speed
1500   = MOTOR_PWM_SLOW (ความเร็วช้า)
3000   = MOTOR_PWM_DEFAULT (ความเร็วปกติ)
```

## Status Bits (Register 20)

| Bit | Flag | Description |
|-----|------|-------------|
| 0 | STATUS_MOTOR_RUNNING | มอเตอร์กำลังทำงาน |
| 1 | STATUS_DISPENSE_ACTIVE | กำลังจ่ายยา |
| 2 | STATUS_HOME_FOUND | พบ HOME position แล้ว |
| 3 | STATUS_CALIBRATED | ระบบ calibrated แล้ว |
| 4 | STATUS_AT_HOME | อยู่ที่ตำแหน่ง HOME |
| 5 | STATUS_HOMING | กำลังทำ HOME |
| 6 | STATUS_CALIBRATING | กำลัง calibrate |
| 7 | STATUS_ERROR | มีข้อผิดพลาด |

## ตัวอย่างการใช้งาน

### 1. การทำ HOME เพียงอย่างเดียว
```
เขียน Register 12 = 1
→ ระบบจะหมุน 1 รอบเพื่อ calibrate แล้วหยุด
→ Register 12 จะถูกล้างเป็น 0 อัตโนมัติ
```

### 2. การจ่ายยาโดยตรง
```
เขียน Register 11 = 10 (จ่าย 10 รอบ)
→ ระบบจะจ่ายยา 10 รอบทันที
→ Register 11 จะถูกล้างเป็น 0 อัตโนมัติ
```

### 3. การทำ HOME + จ่ายยา (Enhanced Homing)
```
เขียน Register 11 = 10 (ตั้งจำนวนรอบ)
เขียน Register 12 = 2 (HOME + Dispense)
→ ระบบจะทำ HOME 1 รอบก่อน
→ หลังจาก HOME เสร็จ จะจ่ายยา 10 รอบ
→ ทั้ง Register 11 และ 12 จะถูกล้างเป็น 0
```

### 4. การตั้งความเร็ว
```
เขียน Register 10 = 1500 (ความเร็วช้า)
เขียน Register 10 = 3000 (ความเร็วปกติ)
```

## การตรวจสอบสถานะ

### ตรวจสอบว่าระบบกำลังทำงาน
```cpp
uint16_t status = อ่าน Register 20
if (status & STATUS_MOTOR_RUNNING) {
    // มอเตอร์กำลังทำงาน
}
if (status & STATUS_HOMING) {
    // กำลังทำ HOME
}
if (status & STATUS_DISPENSE_ACTIVE) {
    // กำลังจ่ายยา
}
```

### ตรวจสอบตำแหน่งปัจจุบัน
```cpp
uint16_t position = อ่าน Register 21
float actualPosition = position / 100.0; // แปลงกลับเป็น float
```

## ลำดับการทำงานของระบบ

### HOME Only (คำสั่ง 1)
1. รับคำสั่ง HOME_CMD_FIND (1)
2. เริ่ม home_start(1, MOTOR_PWM_SLOW)
3. อัปเดต status: STATUS_HOMING = 1
4. รอจนครบ 1 รอบ
5. หยุดและอัปเดต status: STATUS_HOME_FOUND = 1
6. ล้าง Register 12 = 0

### Dispense Only (คำสั่ง dispense)
1. รับคำสั่ง DISPENSE (1-99)
2. เริ่ม dispense_start(rotations, speed)
3. อัปเดต status: STATUS_DISPENSE_ACTIVE = 1
4. รอจนครบจำนวนรอบ
5. หยุดและล้าง status
6. ล้าง Register 11 = 0

### Enhanced Homing (คำสั่ง 2)
1. รับคำสั่ง HOME_CMD_RETURN (2)
2. ตั้งสถานะ Enhanced Homing = true
3. เริ่ม home_start(1, MOTOR_PWM_SLOW)
4. รอจนการ HOME เสร็จ
5. อ่านค่าจาก Register 11 (จำนวนรอบ)
6. เริ่ม dispense_start() ด้วยค่าที่อ่านได้
7. ล้าง Enhanced Homing = false
8. ล้าง Register 11 และ 12 = 0

## ข้อมูล Technical

- **Communication**: Modbus RTU, RS485
- **Slave ID**: 55
- **Baudrate**: 9600, 8N1
- **Polling Rate**: ทุก loop cycle (~1ms)
- **Status Update**: Real-time
- **Auto Clear**: Commands จะถูกล้างอัตโนมัติหลังดำเนินการ

## การแก้ไขปัญหา

### ไม่มีการตอบสนอง
1. ตรวจสอบ Slave ID (55)
2. ตรวจสอบ Baudrate (9600)
3. ตรวจสอบการเชื่อมต่อ RS485

### คำสั่งไม่ทำงาน
1. ตรวจสอบค่า Register ที่ส่ง
2. ดู Serial Monitor สำหรับ debug message
3. ตรวจสอบ Status Register เพื่อดูสถานะปัจจุบัน

### การทำงานผิดปกติ
1. อ่าน Error Register (22)
2. ตรวจสอบ Status bits
3. ดู debug output ใน Serial Monitor

## การใช้งานร่วมกับปุ่ม

ระบบสามารถใช้งานควบคู่กับปุ่มได้:
- **SW_CALC**: HOME + Dispense (เหมือน Modbus command 2)
- **SW_START**: Dispense only (เหมือน Modbus dispense command)
- **Modbus**: ควบคุมระยะไกลพร้อมตรวจสอบสถานะ

ระบบนี้ให้ความยืดหยุ่นสูงในการควบคุม สามารถใช้งานแบบ manual ผ่านปุ่ม หรือควบคุมระยะไกลผ่าน Modbus ได้อย่างสมบูรณ์