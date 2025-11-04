# Drug Dispenser Station - Modbus Tools Usage Guide

## การตั้งค่า Modbus Tools

### Connection Settings:
- **Port**: COM6 (หรือตาม port ที่ device เชื่อมต่อ)
- **Baudrate**: 9600
- **Data bits**: 8
- **Parity**: None (N)
- **Stop bits**: 1
- **Slave ID**: 55

### Register Map:

#### Command Registers (Read/Write):
| Register | Function | Value Range | Description |
|----------|----------|-------------|-------------|
| 10 | Motor Speed | 1000-4000 | PWM speed for motor |
| 11 | Dispense | 1-99 | Number of rotations to dispense |
| 12 | Home Command | 1-2 | 1=Find Home, 2=Return Home |
| 13 | Calibration | 1 | 1=Start Calibration |

#### Status Registers (Read-Only):
| Register | Function | Description |
|----------|----------|-------------|
| 20 | System Status | Bit flags (see below) |
| 21 | Position | Current position × 100 |
| 22 | Error Code | Error status |

### Status Bits (Register 20):
- Bit 0: Motor Running
- Bit 1: Dispensing Active  
- Bit 2: Home Position Found
- Bit 3: System Calibrated
- Bit 4: At Home Position
- Bit 5: Homing in Progress
- Bit 6: Calibrating
- Bit 7: Error Condition

### Error Codes (Register 22):
- 0: No Error
- 1: Home Not Found
- 2: Not Calibrated
- 3: Invalid Command
- 4: Motor Timeout

## Usage Examples:

### 1. Basic Dispensing:
```
1. Write 3000 to Register 10 (set speed)
2. Write 5 to Register 11 (dispense 5 rotations)
```

### 2. Find Home Position:
```
Write 1 to Register 12
```

### 3. Return to Home:
```
Write 2 to Register 12
```

### 4. Check Status:
```
Read Register 20 (status)
Read Register 21 (position)
Read Register 22 (error)
```

## Testing Sequence:

1. **Connect to device**
2. **Read status** (Register 20) - should show Home Found if auto-homing worked
3. **Set speed** (Register 10) = 3000
4. **Dispense** (Register 11) = 5
5. **Monitor status** during operation
6. **Return home** (Register 12) = 2

## Notes:
- System automatically performs homing on startup
- Must have home position before dispensing
- Speed range: 1000-4000 (recommended: 2000-3500)
- Register values auto-clear after execution