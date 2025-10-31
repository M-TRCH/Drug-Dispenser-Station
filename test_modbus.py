#!/usr/bin/env python3
"""
Drug Dispenser Station - Modbus Test Script
ทดสอบระบบ Homing และ Dispensing ผ่าน Modbus RTU

Requirements:
pip install pymodbus
"""

import time
import sys
# Updated imports for pymodbus 3.x
try:
    from pymodbus.client import ModbusSerialClient
    from pymodbus import ModbusException
except ImportError:
    print("❌ Error importing pymodbus. Please install with: pip install pymodbus pyserial")
    sys.exit(1)

# Check if serial is available
try:
    import serial
except ImportError:
    print("❌ pyserial not found. Installing...")
    import subprocess
    subprocess.check_call([sys.executable, "-m", "pip", "install", "pyserial"])
    import serial

class DrugDispenserModbus:
    def __init__(self, port='COM3', baudrate=9600, slave_id=55):
        """เริ่มต้นการเชื่อมต่อ Modbus"""
        # Store port for later use
        self.port = port
        self.baudrate = baudrate
        self.slave_id = slave_id
        
        # Updated syntax for pymodbus 3.x
        self.client = ModbusSerialClient(
            port=port,
            baudrate=baudrate,
            timeout=3,
            parity='N',
            stopbits=1,
            bytesize=8
        )
        
        # Set slave ID for the client (pymodbus 3.x style)
        if hasattr(self.client, 'slave_id'):
            self.client.slave_id = slave_id
        
        # Register addresses
        self.REG_SPEED = 10
        self.REG_DISPENSE = 11
        self.REG_HOME = 12
        self.REG_CALIBRATE = 13
        self.REG_STATUS = 20
        self.REG_POSITION = 21
        self.REG_ERROR = 22
        
        # Command codes
        self.HOME_FIND = 1
        self.HOME_RETURN = 2
        self.CALIB_START = 1
        
        # Status bits
        self.STATUS_MOTOR_RUNNING = 0x01
        self.STATUS_DISPENSE_ACTIVE = 0x02
        self.STATUS_HOME_FOUND = 0x04
        self.STATUS_CALIBRATED = 0x08
        self.STATUS_AT_HOME = 0x10
        self.STATUS_HOMING = 0x20
        self.STATUS_CALIBRATING = 0x40
        self.STATUS_ERROR = 0x80
        
        # Error codes
        self.ERR_NONE = 0
        self.ERR_HOME_NOT_FOUND = 1
        self.ERR_NOT_CALIBRATED = 2
        self.ERR_INVALID_COMMAND = 3
        self.ERR_MOTOR_TIMEOUT = 4
        
    def connect(self):
        """เชื่อมต่อ Modbus"""
        try:
            if self.client.connect():
                print(f"✅ Connected to Modbus device on {self.port}")
                return True
            else:
                print(f"❌ Failed to connect to {self.port}")
                return False
        except Exception as e:
            print(f"❌ Connection error: {e}")
            return False
    
    def disconnect(self):
        """ตัดการเชื่อมต่อ"""
        self.client.close()
        print("🔌 Disconnected")
    
    def read_registers(self, start_addr, count):
        """อ่าน holding registers"""
        try:
            # Set slave ID before reading (pymodbus 3.x style)
            result = self.client.read_holding_registers(start_addr, count)
            if hasattr(result, 'isError') and result.isError():
                print(f"❌ Read error: {result}")
                return None
            if hasattr(result, 'registers'):
                return result.registers
            else:
                print(f"❌ No registers in response: {result}")
                return None
        except Exception as e:
            print(f"❌ Read exception: {e}")
            return None
    
    def write_register(self, addr, value):
        """เขียน holding register"""
        try:
            # Set slave ID before writing (pymodbus 3.x style)
            result = self.client.write_register(addr, value)
            if hasattr(result, 'isError') and result.isError():
                print(f"❌ Write error: {result}")
                return False
            return True
        except Exception as e:
            print(f"❌ Write exception: {e}")
            return False
    
    def get_status(self):
        """อ่านสถานะระบบ"""
        regs = self.read_registers(self.REG_STATUS, 3)
        if regs is None:
            return None
        
        status = regs[0]
        position = regs[1] / 100.0  # Scaled by 100
        error = regs[2]
        
        return {
            'status_raw': status,
            'position': position,
            'error': error,
            'motor_running': bool(status & self.STATUS_MOTOR_RUNNING),
            'dispensing': bool(status & self.STATUS_DISPENSE_ACTIVE),
            'home_found': bool(status & self.STATUS_HOME_FOUND),
            'calibrated': bool(status & self.STATUS_CALIBRATED),
            'at_home': bool(status & self.STATUS_AT_HOME),
            'homing': bool(status & self.STATUS_HOMING),
            'calibrating': bool(status & self.STATUS_CALIBRATING),
            'has_error': bool(status & self.STATUS_ERROR)
        }
    
    def print_status(self):
        """แสดงสถานะระบบ"""
        status = self.get_status()
        if status is None:
            print("❌ Cannot read status")
            return False
        
        print("\n" + "="*50)
        print("🏥 DRUG DISPENSER STATUS")
        print("="*50)
        print(f"📍 Position: {status['position']:.2f} rotations")
        print(f"🏠 Home Found: {'✅ Yes' if status['home_found'] else '❌ No'}")
        print(f"🎯 At Home: {'✅ Yes' if status['at_home'] else '❌ No'}")
        print(f"⚙️  Calibrated: {'✅ Yes' if status['calibrated'] else '❌ No'}")
        print(f"🔄 Motor Running: {'✅ Yes' if status['motor_running'] else '❌ No'}")
        print(f"💊 Dispensing: {'✅ Yes' if status['dispensing'] else '❌ No'}")
        print(f"🔍 Homing: {'🔄 In Progress' if status['homing'] else '⏹️  Idle'}")
        print(f"🛠️  Calibrating: {'🔄 In Progress' if status['calibrating'] else '⏹️  Idle'}")
        
        if status['has_error']:
            error_msgs = {
                self.ERR_HOME_NOT_FOUND: "Home position not found",
                self.ERR_NOT_CALIBRATED: "System not calibrated", 
                self.ERR_INVALID_COMMAND: "Invalid command",
                self.ERR_MOTOR_TIMEOUT: "Motor timeout"
            }
            error_msg = error_msgs.get(status['error'], f"Unknown error ({status['error']})")
            print(f"⚠️  Error: {error_msg}")
        else:
            print("✅ No errors")
        
        print("="*50)
        return True
    
    def find_home(self):
        """หาตำแหน่ง Home"""
        print("🔍 Finding home position...")
        if not self.write_register(self.REG_HOME, self.HOME_FIND):
            return False
        
        # รอให้เสร็จ
        timeout = 30  # 30 seconds timeout
        start_time = time.time()
        
        while time.time() - start_time < timeout:
            status = self.get_status()
            if status is None:
                return False
                
            if not status['homing']:  # ไม่ได้ homing แล้ว
                if status['home_found']:
                    print("✅ Home position found!")
                    return True
                else:
                    print("❌ Failed to find home position")
                    return False
            
            print("🔄 Homing in progress...")
            time.sleep(1)
        
        print("⏰ Homing timeout")
        return False
    
    def return_home(self):
        """กลับไปตำแหน่ง Home"""
        print("🏠 Returning to home position...")
        if not self.write_register(self.REG_HOME, self.HOME_RETURN):
            return False
        
        # รอให้เสร็จ
        timeout = 30
        start_time = time.time()
        
        while time.time() - start_time < timeout:
            status = self.get_status()
            if status is None:
                return False
                
            if status['at_home']:
                print("✅ Returned to home position!")
                return True
            
            if not status['motor_running']:
                print("⚠️  Motor stopped, checking position...")
                time.sleep(1)
                status = self.get_status()
                if status and status['at_home']:
                    print("✅ At home position!")
                    return True
                else:
                    print("❌ Not at home position")
                    return False
            
            print(f"🔄 Moving to home... (pos: {status['position']:.2f})")
            time.sleep(1)
        
        print("⏰ Return home timeout")
        return False
    
    def calibrate(self):
        """สั่ง Calibration"""
        print("🛠️  Starting system calibration...")
        if not self.write_register(self.REG_CALIBRATE, self.CALIB_START):
            return False
        
        # รอให้เสร็จ
        timeout = 60  # 60 seconds for calibration
        start_time = time.time()
        
        while time.time() - start_time < timeout:
            status = self.get_status()
            if status is None:
                return False
                
            if not status['calibrating']:  # ไม่ได้ calibrating แล้ว
                if status['calibrated']:
                    print("✅ System calibration completed!")
                    return True
                else:
                    print("❌ Calibration failed")
                    return False
            
            print("🔄 Calibration in progress...")
            time.sleep(2)
        
        print("⏰ Calibration timeout")
        return False
    
    def dispense(self, rotations, speed=3000):
        """จ่ายยา"""
        print(f"💊 Dispensing {rotations} rotations at speed {speed}...")
        
        # ตั้งค่าความเร็ว
        if not self.write_register(self.REG_SPEED, speed):
            return False
        
        # สั่งจ่ายยา
        if not self.write_register(self.REG_DISPENSE, rotations):
            return False
        
        # รอให้เสร็จ
        timeout = 60
        start_time = time.time()
        
        while time.time() - start_time < timeout:
            status = self.get_status()
            if status is None:
                return False
                
            if not status['dispensing'] and not status['motor_running']:
                print("✅ Dispensing completed!")
                return True
            
            print(f"🔄 Dispensing... (pos: {status['position']:.2f})")
            time.sleep(1)
        
        print("⏰ Dispensing timeout")
        return False

def main():
    """ฟังก์ชันหลักสำหรับทดสอบ"""
    print("🏥 Drug Dispenser Modbus Test")
    print("=" * 40)
    
    # เปลี่ยน COM port ตามที่ใช้จริง (ปรับตาม RS485 port ของคุณ)
    dispenser = DrugDispenserModbus(port='COM6', baudrate=9600)  # เปลี่ยนเป็น COM6 ตาม Upload log
    
    if not dispenser.connect():
        return
    
    try:
        while True:
            print("\n📋 MENU:")
            print("1. Show Status")
            print("2. Find Home Position") 
            print("3. Return to Home")
            print("4. Calibrate System")
            print("5. Dispense Pills")
            print("6. Quick Test Sequence")
            print("0. Exit")
            
            choice = input("\nSelect option (0-6): ").strip()
            
            if choice == '0':
                break
            elif choice == '1':
                dispenser.print_status()
            elif choice == '2':
                dispenser.find_home()
            elif choice == '3':
                dispenser.return_home()
            elif choice == '4':
                dispenser.calibrate()
            elif choice == '5':
                try:
                    rotations = int(input("Enter rotations (1-99): "))
                    speed = int(input("Enter speed (1000-4000, default 3000): ") or "3000")
                    dispenser.dispense(rotations, speed)
                except ValueError:
                    print("❌ Invalid input")
            elif choice == '6':
                print("\n🚀 Running Quick Test Sequence...")
                print("Step 1: Check initial status")
                dispenser.print_status()
                
                print("\nStep 2: Find home position")
                if dispenser.find_home():
                    print("\nStep 3: Calibrate system")
                    if dispenser.calibrate():
                        print("\nStep 4: Test dispense 2 rotations")
                        if dispenser.dispense(2, 2500):
                            print("\nStep 5: Return to home")
                            dispenser.return_home()
                            print("\n✅ Quick test completed successfully!")
                        else:
                            print("❌ Dispense test failed")
                    else:
                        print("❌ Calibration failed")
                else:
                    print("❌ Home finding failed")
            else:
                print("❌ Invalid option")
                
    except KeyboardInterrupt:
        print("\n🛑 Test interrupted by user")
    except Exception as e:
        print(f"❌ Unexpected error: {e}")
    finally:
        dispenser.disconnect()

if __name__ == "__main__":
    main()