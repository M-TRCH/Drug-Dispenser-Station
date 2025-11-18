#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Simple RS232 Speed Test
ทดสอบความเร็ว RS232 แบบง่ายๆ

Requirements:
pip install pyserial
"""

import serial
import serial.tools.list_ports
import time

def list_ports():
    """แสดง COM ports ที่มี"""
    ports = serial.tools.list_ports.comports()
    print("COM Ports ที่พร้อมใช้:")
    for i, port in enumerate(ports):
        print(f"{i+1}. {port.device} - {port.description}")
    return ports

def simple_speed_test(port_name, baudrate, duration=10):
    """ทดสอบความเร็วแบบง่าย"""
    print(f"\nทดสอบ {port_name} ที่ {baudrate} bps เป็นเวลา {duration} วินาที")
    
    try:
        # เชื่อมต่อ
        ser = serial.Serial(port_name, baudrate, timeout=1)
        time.sleep(1)  # รอให้เชื่อมต่อ
        
        # ข้อมูลทดสอบ
        test_data = b'0123456789ABCDEF' * 64  # 1024 bytes
        
        bytes_sent = 0
        start_time = time.time()
        end_time = start_time + duration
        
        # ส่งข้อมูลต่อเนื่อง
        while time.time() < end_time:
            ser.write(test_data)
            bytes_sent += len(test_data)
            time.sleep(0.01)  # หน่วงเล็กน้อย
        
        actual_time = time.time() - start_time
        
        # คำนวณผลลัพธ์
        bits_sent = bytes_sent * 8
        speed_bps = bits_sent / actual_time
        speed_kbps = speed_bps / 1000
        efficiency = (speed_bps / baudrate) * 100
        
        print(f"✓ สำเร็จ!")
        print(f"  ข้อมูลที่ส่ง: {bytes_sent:,} bytes ({bits_sent:,} bits)")
        print(f"  ความเร็วจริง: {speed_bps:,.0f} bps ({speed_kbps:.2f} Kbps)")
        print(f"  ประสิทธิภาพ: {efficiency:.1f}%")
        
        ser.close()
        return True
        
    except Exception as e:
        print(f"✗ ผิดพลาด: {e}")
        return False

def main():
    print("=== Simple RS232 Speed Test ===")
    
    # แสดง ports
    ports = list_ports()
    if not ports:
        print("ไม่พบ COM port")
        return
    
    # เลือก port
    try:
        choice = int(input(f"\nเลือก port (1-{len(ports)}): ")) - 1
        port = ports[choice].device
    except:
        print("การเลือกไม่ถูกต้อง")
        return
    
    # ทดสอบ baudrates ต่างๆ
    baudrates = [9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600]
    
    print(f"\nทดสอบ {port}...")
    max_working = 0
    
    for baudrate in baudrates:
        if simple_speed_test(port, baudrate, 5):
            max_working = baudrate
        else:
            print(f"หยุดที่ {baudrate} bps")
            break
    
    print(f"\n🎯 Baudrate สูงสุดที่ทำงานได้: {max_working:,} bps")

if __name__ == "__main__":
    main()