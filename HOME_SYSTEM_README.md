# ระบบ HOME สำหรับ Drug Dispenser Station

## ภาพรวมของระบบ

ระบบ HOME ถูกออกแบบมาเพื่อให้การจ่ายยามีความแม่นยำมากขึ้น โดยจะทำการ "calibrate" ตำแหน่งเริ่มต้นก่อนการจ่ายยาเมื่อต้องการ

## การทำงานของระบบ

### ปุ่มและฟังก์ชัน

1. **ปุ่ม SW_CALC (PA0) - ปุ่ม HOME + DISPENSE**
   - กดปุ่มนี้เพื่อเริ่มกระบวนการ HOME + จ่ายยา
   - ลำดับการทำงาน:
     1. หมุน 1 รอบด้วยความเร็วช้า (MOTOR_PWM_SLOW) เพื่อ HOME (calibration)
     2. รอจนกว่า HOME จะเสร็จสิ้น (blocking wait)
     3. หลังจาก HOME เสร็จ จะจ่ายยาด้วยความเร็วปกติ (MOTOR_PWM_DEFAULT)

2. **ปุ่ม SW_START (PA1) - ปุ่ม DISPENSE โดยตรง**
   - กดปุ่มนี้เพื่อจ่ายยาโดยตรง (ไม่ผ่าน HOME)
   - จ่ายยาด้วยความเร็วปกติ (MOTOR_PWM_DEFAULT) ตามค่าที่กำหนดทันที

### การกำหนดค่า

ในไฟล์ `main.cpp` สามารถปรับค่าได้:

```cpp
#define DISPENSE_ROTATIONS 10         // จำนวนรอบที่จะจ่าย
#define HOME_ROTATIONS 1              // จำนวนรอบสำหรับ HOME (ใน main.cpp)
```

ในไฟล์ `include/dispense.h`:
```cpp
#define HOME_MAX_ROTATIONS 2          // ค่าสูงสุดสำหรับ HOME
```

### ตัวแปรสำคัญ

```cpp
// ในไฟล์ dispense.h และ dispense.cpp
bool homeActive;                      // สถานะการทำ HOME
bool homeCompleted;                   // สถานะความสำเร็จของ HOME
volatile uint32_t rotationCounter;    // ตัวนับรอบปัจจุบัน
uint32_t targetRotations;             // เป้าหมายจำนวนรอบ
```

## ฟังก์ชันที่อัปเดตแล้ว

### ในไฟล์ dispense.h
```cpp
void home_start(uint32_t rotations, int motorSpeed = MOTOR_PWM_SLOW); // เริ่มการทำ HOME พร้อมกำหนดรอบและความเร็ว
void home_stop();                         // หยุดการทำ HOME  
bool is_home_completed();                 // ตรวจสอบว่า HOME เสร็จแล้วหรือไม่
```

### การทำงานของ ISR
```cpp
void _dispenseISR() {
    rotationCounter++;
    
    // ตรวจสอบ HOME ก่อน (ใช้ HOME_MAX_ROTATIONS สำหรับ safety)
    if (homeActive && rotationCounter >= HOME_MAX_ROTATIONS) {
        home_stop();
        return;
    }
    
    // ตรวจสอบ DISPENSE
    if (rotationCounter >= targetRotations && dispenseActive) {
        dispense_stop();
    }
}
```

## ลำดับการทำงาน

### การกด SW_CALC (HOME + DISPENSE)
1. ผู้ใช้กดปุ่ม SW_CALC
2. ระบบเริ่ม HOME: `home_start(1, MOTOR_PWM_SLOW)` - หมุน 1 รอบด้วยความเร็วช้า
3. รอให้ HOME เสร็จสิ้นด้วย blocking while loop
4. เริ่มจ่ายยา: `dispense_start(DISPENSE_ROTATIONS, MOTOR_PWM_DEFAULT)`
5. หยุดเมื่อครบตามจำนวนรอบ

### การกด SW_START (DISPENSE โดยตรง)
1. ผู้ใช้กดปุ่ม SW_START  
2. จ่ายยาทันที: `dispense_start(DISPENSE_ROTATIONS, MOTOR_PWM_DEFAULT)`
3. หยุดเมื่อครบตามจำนวนรอบ

## คุณสมบัติการ Debug

ระบบมี debug output ทุก 2 วินาที:
```cpp
Serial.printf("[Debug] SW_CALC: %d, SW_START: %d\n", 
             digitalRead(SW_CALC_PIN), digitalRead(SW_START_PIN));
```

## ข้อควรระวัง

1. **Safety Checks**: ระบบมีการตรวจสอบความปลอดภัยต่อเนื่องใน `dispense_update()`
2. **Debounce**: มีการป้องกัน bounce ของปุ่มด้วย delay 500ms
3. **Blocking Wait**: การรอ HOME ใช้ while loop ที่ยังคงเรียก safety checks ทุก 500ms
4. **Error Handling**: ระบบจะหยุดทำงานหากมอเตอร์หยุดโดยไม่คาดคิด
5. **Parameter Validation**: ทั้ง `home_start()` และ `dispense_start()` ตรวจสอบพารามิเตอร์ก่อนใช้งาน

## การทดสอบ

1. **ทดสอบ HOME**: กดปุ่ม SW_CALC และสังเกตว่า:
   - มอเตอร์หมุน 1 รอบด้วยความเร็วช้าแล้วหยุด
   - จากนั้นหมุนต่อ 10 รอบด้วยความเร็วปกติ
   
2. **ทดสอบ DISPENSE**: กดปุ่ม SW_START และสังเกตว่า:
   - มอเตอร์หมุน 10 รอบด้วยความเร็วปกติโดยตรง
   
3. **ติดตาม Debug**: ดู Serial Monitor เพื่อ:
   - ติดตามสถานะปุ่มทุก 2 วินาที
   - ติดตามข้อความการทำงานของระบบ

## การปรับแต่ง

- **จำนวนรอบจ่าย**: แก้ไข `DISPENSE_ROTATIONS` ในไฟล์ main.cpp
- **จำนวนรอบ HOME**: แก้ไข `HOME_ROTATIONS` ในไฟล์ main.cpp  
- **ความเร็ว HOME**: แก้ไขพารามิเตอร์ที่สองใน `home_start()` 
- **ความเร็ว DISPENSE**: แก้ไขพารามิเตอร์ที่สองใน `dispense_start()`
- **ค่าสูงสุด HOME**: แก้ไข `HOME_MAX_ROTATIONS` ในไฟล์ dispense.h

## ไฟล์ที่แก้ไข

1. `include/dispense.h` - อัปเดตฟังก์ชัน HOME ให้รับพารามิเตอร์
2. `src/dispense.cpp` - เพิ่มการ validate พารามิเตอร์ใน home_start()
3. `src/main.cpp` - เพิ่มการจัดการปุ่ม, debug output และ blocking wait

## ข้อมูล Technical

- **HOME Speed**: MOTOR_PWM_SLOW (1500) - ความเร็วช้าสำหรับความแม่นยำ
- **Dispense Speed**: MOTOR_PWM_DEFAULT (3000) - ความเร็วปกติ
- **Sensor**: SEN_1_PIN (PB9) - เซนเซอร์ตรวจจับรอบ (1 PPR)
- **Polling Rate**: ทุก 10ms สำหรับ safety checks
- **Debug Rate**: ทุก 2000ms สำหรับ status display

ระบบนี้ให้ความยืดหยุ่นในการใช้งาน โดยผู้ใช้สามารถเลือกได้ว่าจะให้ระบบทำ HOME ก่อนจ่ายยา หรือจ่ายยาโดยตรง พร้อมทั้งมีระบบ debug เพื่อช่วยในการแก้ไขปัญหา
    rotationCounter++;
    
    // ตรวจสอบ HOME ก่อน
    if (homeActive && rotationCounter >= HOME_ROTATIONS) {
        home_stop();
        return;
    }
    
    // ตรวจสอบ DISPENSE
    if (rotationCounter >= targetRotations && dispenseActive) {
        dispense_stop();
    }
}
```

## ลำดับการทำงาน

### การกด SW_CALC (HOME + DISPENSE)
1. ผู้ใช้กดปุ่ม SW_CALC
2. ระบบเริ่ม HOME: หมุน 1 รอบ
3. รอให้ HOME เสร็จสิ้น
4. เริ่มจ่ายยาตามค่าที่กำหนด
5. หยุดเมื่อครบตามจำนวนรอบ

### การกด SW_START (DISPENSE โดยตรง)
1. ผู้ใช้กดปุ่ม SW_START  
2. จ่ายยาทันทีตามค่าที่กำหนด
3. หยุดเมื่อครบตามจำนวนรอบ

## ข้อควรระวัง

1. **Safety Checks**: ระบบมีการตรวจสอบความปลอดภัยต่อเนื่อง
2. **Debounce**: มีการป้องกัน bounce ของปุ่มด้วย delay 500ms
3. **Non-blocking**: การรอ HOME ใช้ while loop ที่ยังคงเรียก safety checks
4. **Error Handling**: ระบบจะหยุดทำงานหากมอเตอร์หยุดโดยไม่คาดคิด

## การทดสอบ

1. **ทดสอบ HOME**: กดปุ่ม SW_CALC และสังเกตว่ามอเตอร์หมุน 1 รอบแล้วหยุด จากนั้นหมุนต่อตามจำนวนที่กำหนด
2. **ทดสอบ DISPENSE**: กดปุ่ม SW_START และสังเกตว่ามอเตอร์หมุนตามจำนวนที่กำหนดโดยตรง
3. **ติดตาม Serial**: ดู Serial Monitor เพื่อติดตามสถานะการทำงาน

## การปรับแต่ง

- **จำนวนรอบจ่าย**: แก้ไข `DISPENSE_ROTATIONS` ในไฟล์ main.cpp
- **ความเร็ว**: แก้ไข `DISPENSE_SPEED` ในไฟล์ main.cpp  
- **HOME รอบ**: แก้ไข `HOME_ROTATIONS` ในไฟล์ dispense.h (ปกติควรเป็น 1)

## ไฟล์ที่แก้ไข

1. `include/dispense.h` - เพิ่มฟังก์ชันและตัวแปร HOME
2. `src/dispense.cpp` - เพิ่มการทำงาน HOME และ ISR ใหม่
3. `src/main.cpp` - เพิ่มการจัดการปุ่มและลำดับการทำงาน

ระบบนี้ให้ความยืดหยุ่นในการใช้งาน โดยผู้ใช้สามารถเลือกได้ว่าจะให้ระบบทำ HOME ก่อนจ่ายยา หรือจ่ายยาโดยตรง