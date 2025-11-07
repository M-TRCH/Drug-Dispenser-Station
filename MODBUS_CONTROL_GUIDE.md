# คู่มือควบคุม Drug Dispenser ผ่าน Modbus

# ระบบนี้ควบคุมได้ 4 แบบ: **ปุ่ม manual**, **HOME อย่างเดียว**, **จ่ายยาตรง**, **HOME+จ่าย** และ **หาจุด HOME ใหม่**

## การตั้งค่าเบื้องต้น

- **Slave ID**: 55
- **Baudrate**: 9600, 8N1  
- **Protocol**: Modbus RTU over RS485

## Register Map

### คำสั่งควบคุม (เขียนได้)

| Address | Function | Values | Description |
|---------|----------|--------|-------------|
| **10** | ความเร็ว | 0-4095 | PWM speed (1500=ช้า, 3000=ปกติ) |
| **11** | จ่ายยา | 1-99 | จำนวนรอบที่ต้องการ |
| **12** | HOME | 0,1,2,3 | 0 = หยุด, 1 = HOME, 2 = HOME+จ่าย, 3 =Check HOME(ถ้าไม่ตรงให้ทำการหา, ถ้าตรงอยู่แล้วไม่ต้อง Home ซ้ำ)|

### สถานะระบบ (อ่านอย่างเดียว)

| Address | Function | Description |
|---------|----------|-------------|
| **20** | สถานะ | Bit flags (ดูตารางด้านล่าง) |
| **21** | ตำแหน่ง | ตำแหน่งปัจจุบัน ×100 |
| **22** | Error | รหัสข้อผิดพลาด |

## วิธีใช้งาน

### 1. HOME อย่างเดียว
```
เขียน Register 12 = 1
→ หมุน 1 รอบเพื่อ calibrate แล้วหยุด
```

### 2. จ่ายยาตรง ๆ
```
เขียน Register 11 = 10
→ จ่ายยา 10 รอบทันที
```

### 3. HOME แล้วจ่ายยา
```
เขียน Register 11 = 10  (ตั้งจำนวนรอบ)
เขียน Register 12 = 2   (HOME + จ่าย)
→ HOME ก่อน แล้วจ่าย 10 รอบ
```

### 4. หาจุด HOME ใหม่ (กรณีหยุดกลางทาง)
```
เขียน Register 12 = 3
→ วิ่งจนกว่าจะเจอเซนเซอร์ (timeout 10 วินาที)
```

## Status Bits (Register 20) - สถานะการทำงาน

ระบบใช้ 8 bits ในการแสดงสถานะ อ่านค่าแล้วใช้ bitwise AND (&) เพื่อตรวจสอบแต่ละ bit

### รายละเอียด Status Bits

| Bit | Flag | เมื่อไหร่ที่เป็น 1 | ตัวอย่างการใช้งาน |
|-----|------|-------------------|-------------------|
| **0** | MOTOR_RUNNING | มอเตอร์กำลังหมุน | ตรวจสอบว่าเครื่องทำงานอยู่หรือไม่ |
| **1** | DISPENSE_ACTIVE | กำลังจ่ายยา | ตรวจสอบว่าการจ่ายยาเสร็จหรือยัง |
| **2** | HOME_FOUND | เจอตำแหน่ง HOME แล้ว | ยืนยันว่า calibration สำเร็จ |
| **3** | CALIBRATED | ระบบพร้อมใช้งาน | ตรวจสอบว่าเครื่องพร้อมรับคำสั่งหรือไม่ |
| **4** | AT_HOME | อยู่ที่ตำแหน่ง HOME | ตรวจสอบตำแหน่งปัจจุบัน |
| **5** | HOMING | กำลังทำ HOME/หาเซนเซอร์ | ตรวจสอบว่า HOME เสร็จหรือยัง |
| **6** | CALIBRATING | กำลัง Enhanced Homing | ตรวจสอบ HOME+Dispense sequence |
| **7** | ERROR | มีข้อผิดพลาด | ตรวจสอบว่าระบบมีปัญหาหรือไม่ |

### การอ่าน Status Bits

```python
# อ่านค่า status
status = read_register(20)

# ตรวจสอบแต่ละ bit
if status & 0x01:  # bit 0
    print("มอเตอร์กำลังทำงาน")
    
if status & 0x02:  # bit 1  
    print("กำลังจ่ายยา")
    
if status & 0x04:  # bit 2
    print("เจอ HOME position แล้ว")
    
if status & 0x20:  # bit 5
    print("กำลัง HOME")
    
if status & 0x80:  # bit 7
    print("มี Error! ดู Error Code ที่ Register 22")
```

### ตัวอย่าง Status Values

| Value | Binary | ความหมาย |
|-------|--------|-----------|
| 0 | 00000000 | ระบบหยุดนิ่ง |
| 1 | 00000001 | มอเตอร์ทำงาน |
| 32 | 00100000 | กำลังทำ HOME (bit 5) |
| 34 | 00100010 | กำลังทำ HOME + จ่ายยา (bit 5,1) |
| 128 | 10000000 | มี Error (bit 7) |

## Error Codes (Register 22) - รหัสข้อผิดพลาด

| Code | ชื่อ Error | สาเหตุ | วิธีแก้ไข |
|------|------------|-------|----------|
| **0** | ERR_NONE | ไม่มีข้อผิดพลาด | ระบบทำงานปกติ |
| **1** | ERR_HOME_NOT_FOUND | หา HOME ไม่เจอ | ตรวจสอบเซนเซอร์ HOME |
| **2** | ERR_NOT_CALIBRATED | ระบบไม่ได้ calibrate | ทำ HOME ก่อนใช้งาน |
| **3** | ERR_INVALID_COMMAND | คำสั่งไม่ถูกต้อง | ตรวจสอบค่า Register ที่ส่ง |
| **4** | ERR_MOTOR_TIMEOUT | มอเตอร์ timeout | ตรวจสอบการเชื่อมต่อมอเตอร์ |
| **5** | ERR_HOME_SEEK_TIMEOUT | หาเซนเซอร์ timeout (>10วิ) | ตรวจสอบเซนเซอร์และตำแหน่ง |

### การแก้ไข Error

1. **อ่าน Error Code**
   ```python
   error_code = read_register(22)
   if error_code != 0:
       print(f"Error Code: {error_code}")
   ```

2. **แก้ไขตาม Error Code**
   - **Code 1,5**: ตรวจสอบเซนเซอร์ HOME (SEN_1_PIN)
   - **Code 2**: ทำ HOME ด้วยคำสั่ง Register 12 = 1
   - **Code 3**: ตรวจสอบค่าที่ส่งใน Register 11,12
   - **Code 4**: ตรวจสอบการเชื่อมต่อมอเตอร์

3. **Reset Error**
   - เริ่มการทำงานใหม่จะ clear error อัตโนมัติ
   - หรือปิดเปิดระบบใหม่

## การแก้ปัญหา

### ไม่ตอบสนอง
- ตรวจสอบ Slave ID = 55
- ตรวจสอบ Baudrate = 9600
- ตรวจสอบสาย RS485

### คำสั่งไม่ทำงาน  
- ดู Serial Monitor
- อ่าน Status Register 20
- อ่าน Error Register 22

## Tips

- **Commands จะถูกล้างอัตโนมัติ** หลังดำเนินการเสร็จ
- **ใช้ร่วมกับปุ่ม** ได้ปกติ
- **Status อัปเดตแบบ real-time**
- **Timeout การหาเซนเซอร์**: 10 วินาที

## ตัวอย่างการใช้งาน

```python
# Python example
import modbus_tk.modbus_rtu as modbus_rtu

# เชื่อมต่อ
master = modbus_rtu.RtuMaster()
master.open()

# HOME + จ่ายยา 5 รอบ
master.execute(55, 6, 11, 5)      # ตั้งจำนวนรอบ
master.execute(55, 6, 12, 2)      # HOME + จ่าย

# ตรวจสอบสถานะ
status = master.execute(55, 3, 20, 1)[0]
if status & 0x20:  # bit 5
    print("กำลังทำ HOME")
```
