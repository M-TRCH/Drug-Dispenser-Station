#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Quick RS232 Speed Check
ทดสอบความเร็ว RS232 แบบด่วน

สำหรับผู้ที่ต้องการทดสอบเร็วๆ

Usage: python quick_test.py COM3 115200
"""

import serial
import time
import sys

def quick_test(port, baudrate, duration=5):
    """ทดสอบเร็ว"""
    try:
        print(f"ทดสอบ {port} ที่ {baudrate:,} bps...")
        
        # เชื่อมต่อ
        ser = serial.Serial(port, baudrate, timeout=1)
        time.sleep(0.5)
        
        # ส่งข้อมูล
        data = b'X' * 1024  # 1KB
        bytes_sent = 0
        start = time.time()
        
        while time.time() - start < duration:
            ser.write(data)
            bytes_sent += len(data)
        
        elapsed = time.time() - start
        speed = (bytes_sent * 8) / elapsed
        efficiency = (speed / baudrate) * 100
        
        print(f"✓ ความเร็ว: {speed:,.0f} bps ({speed/1000:.1f} Kbps)")
        print(f"✓ ประสิทธิภาพ: {efficiency:.1f}%")
        
        ser.close()
        return True
        
    except Exception as e:
        print(f"✗ ข้อผิดพลาด: {e}")
        return False

def main():
    if len(sys.argv) == 3:
        port = sys.argv[1]
        baudrate = int(sys.argv[2])
        quick_test(port, baudrate)
    else:
        print("Usage: python quick_test.py COM3 115200")
        
        # แสดง ports ที่มี
        import serial.tools.list_ports
        ports = serial.tools.list_ports.comports()
        if ports:
            print("\nCOM Ports ที่มี:")
            for port in ports:
                print(f"  {port.device}")

if __name__ == "__main__":
    main()