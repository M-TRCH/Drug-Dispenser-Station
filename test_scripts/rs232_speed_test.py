#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
RS232 Speed Test Tool
ทดสอบความเร็วของการสื่อสาร RS232 ผ่าน COM port

Requirements:
pip install pyserial

Usage:
python rs232_speed_test.py
"""

import serial
import serial.tools.list_ports
import time
import threading
import os
import sys

class RS232SpeedTest:
    def __init__(self):
        self.port = None
        self.ser = None
        self.is_testing = False
        
    def list_com_ports(self):
        """แสดงรายการ COM port ที่พร้อมใช้งาน"""
        ports = serial.tools.list_ports.comports()
        print("\n=== COM Ports ที่พร้อมใช้งาน ===")
        if not ports:
            print("ไม่พบ COM port")
            return []
        
        for i, port in enumerate(ports):
            print(f"{i+1}. {port.device} - {port.description}")
        return ports
    
    def select_port(self):
        """เลือก COM port"""
        ports = self.list_com_ports()
        if not ports:
            return False
            
        while True:
            try:
                choice = input(f"\nเลือก COM port (1-{len(ports)}): ")
                index = int(choice) - 1
                if 0 <= index < len(ports):
                    self.port = ports[index].device
                    print(f"เลือก: {self.port}")
                    return True
                else:
                    print("หมายเลขไม่ถูกต้อง")
            except ValueError:
                print("กรุณาใส่หมายเลข")
            except KeyboardInterrupt:
                print("\nยกเลิก")
                return False
    
    def test_baudrate(self, baudrate, test_duration=10, data_size=1024):
        """ทดสอบความเร็วที่ baudrate ที่กำหนด"""
        print(f"\n--- ทดสอบที่ {baudrate} bps ---")
        
        try:
            # เชื่อมต่อ serial port
            self.ser = serial.Serial(
                port=self.port,
                baudrate=baudrate,
                bytesize=serial.EIGHTBITS,
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE,
                timeout=1,
                xonxoff=False,
                rtscts=False,
                dsrdtr=False
            )
            
            print(f"เชื่อมต่อสำเร็จที่ {baudrate} bps")
            time.sleep(0.5)  # รอให้เชื่อมต่อเสถียร
            
            # สร้างข้อมูลทดสอบ
            test_data = b'A' * data_size  # ส่งตัว A ขนาด data_size bytes
            
            # ทดสอบการส่งข้อมูล
            bytes_sent = 0
            bytes_received = 0
            start_time = time.time()
            test_end_time = start_time + test_duration
            
            print(f"ส่งข้อมูล {data_size} bytes ซ้ำๆ เป็นเวลา {test_duration} วินาที...")
            
            # เคลียร์ buffer
            self.ser.reset_input_buffer()
            self.ser.reset_output_buffer()
            
            while time.time() < test_end_time:
                try:
                    # ส่งข้อมูล
                    self.ser.write(test_data)
                    bytes_sent += len(test_data)
                    
                    # อ่านข้อมูลที่ส่งกลับมา (หากมี loopback)
                    if self.ser.in_waiting > 0:
                        received = self.ser.read(self.ser.in_waiting)
                        bytes_received += len(received)
                    
                    time.sleep(0.001)  # หน่วงเล็กน้อย
                    
                except serial.SerialException as e:
                    print(f"ข้อผิดพลาด: {e}")
                    break
            
            actual_time = time.time() - start_time
            
            # คำนวณผลลัพธ์
            send_speed_bps = (bytes_sent * 8) / actual_time
            send_speed_kbps = send_speed_bps / 1000
            
            if bytes_received > 0:
                receive_speed_bps = (bytes_received * 8) / actual_time
                receive_speed_kbps = receive_speed_bps / 1000
            else:
                receive_speed_bps = 0
                receive_speed_kbps = 0
            
            # แสดงผลลัพธ์
            print(f"เวลาทดสอบ: {actual_time:.2f} วินาที")
            print(f"ข้อมูลที่ส่ง: {bytes_sent:,} bytes")
            print(f"ข้อมูลที่รับ: {bytes_received:,} bytes")
            print(f"ความเร็วส่ง: {send_speed_bps:,.0f} bps ({send_speed_kbps:.2f} Kbps)")
            if bytes_received > 0:
                print(f"ความเร็วรับ: {receive_speed_bps:,.0f} bps ({receive_speed_kbps:.2f} Kbps)")
            
            # เปรียบเทียบกับ baudrate ที่ตั้งค่า
            efficiency = (send_speed_bps / baudrate) * 100
            print(f"ประสิทธิภาพ: {efficiency:.1f}% ของ baudrate ที่ตั้งค่า")
            
            self.ser.close()
            return True
            
        except serial.SerialException as e:
            print(f"ไม่สามารถเชื่อมต่อที่ {baudrate} bps: {e}")
            return False
        except Exception as e:
            print(f"ข้อผิดพลาด: {e}")
            return False
    
    def loopback_test(self, baudrate, test_duration=5):
        """ทดสอบ loopback (TX เชื่อมต่อกับ RX)"""
        print(f"\n--- ทดสอบ Loopback ที่ {baudrate} bps ---")
        print("หมายเหตุ: ต้องเชื่อม pin TX กับ pin RX เข้าด้วยกัน")
        
        try:
            self.ser = serial.Serial(
                port=self.port,
                baudrate=baudrate,
                timeout=1
            )
            
            test_message = b"Hello RS232 Test!"
            success_count = 0
            total_tests = 0
            start_time = time.time()
            
            while time.time() - start_time < test_duration:
                # ส่งข้อมูล
                self.ser.write(test_message)
                total_tests += 1
                
                # รอรับข้อมูล
                time.sleep(0.01)
                if self.ser.in_waiting >= len(test_message):
                    received = self.ser.read(len(test_message))
                    if received == test_message:
                        success_count += 1
                
                time.sleep(0.1)
            
            success_rate = (success_count / total_tests) * 100 if total_tests > 0 else 0
            print(f"ทดสอบทั้งหมด: {total_tests} ครั้ง")
            print(f"สำเร็จ: {success_count} ครั้ง")
            print(f"อัตราสำเร็จ: {success_rate:.1f}%")
            
            self.ser.close()
            
        except Exception as e:
            print(f"ข้อผิดพลาด: {e}")
    
    def comprehensive_test(self):
        """ทดสอบหลาย baudrate"""
        if not self.select_port():
            return
        
        # รายการ baudrate ที่จะทดสอบ
        baudrates = [
            9600, 19200, 38400, 57600, 115200,
            230400, 460800, 921600, 1000000, 2000000
        ]
        
        print(f"\n=== ทดสอบความเร็ว RS232 บน {self.port} ===")
        print("การทดสอบจะส่งข้อมูลต่อเนื่องและวัดความเร็วจริง")
        
        results = []
        
        for baudrate in baudrates:
            print(f"\n{'='*50}")
            success = self.test_baudrate(baudrate, test_duration=5, data_size=512)
            results.append((baudrate, success))
            
            if not success:
                print(f"Baudrate {baudrate} ใช้งานไม่ได้ - หยุดทดสอบ")
                break
        
        # สรุปผลลัพธ์
        print(f"\n{'='*50}")
        print("=== สรุปผลการทดสอบ ===")
        for baudrate, success in results:
            status = "✓ ใช้งานได้" if success else "✗ ใช้งานไม่ได้"
            print(f"{baudrate:>10} bps: {status}")
        
        # หา baudrate สูงสุดที่ใช้งานได้
        working_baudrates = [br for br, success in results if success]
        if working_baudrates:
            max_baudrate = max(working_baudrates)
            print(f"\nBaudrate สูงสุดที่ใช้งานได้: {max_baudrate:,} bps")
        
        # เสนอทดสอบ loopback
        if working_baudrates:
            test_loopback = input("\nต้องการทดสอบ loopback หรือไม่? (y/n): ")
            if test_loopback.lower() == 'y':
                self.loopback_test(max_baudrate)

def main():
    print("=" * 60)
    print("    RS232 Speed Test Tool")
    print("    ทดสอบความเร็วของการสื่อสาร RS232")
    print("=" * 60)
    
    tester = RS232SpeedTest()
    
    try:
        tester.comprehensive_test()
    except KeyboardInterrupt:
        print("\n\nยกเลิกการทดสอบ")
    except Exception as e:
        print(f"\nเกิดข้อผิดพลาด: {e}")
    
    input("\nกด Enter เพื่อออกจากโปรแกรม...")

if __name__ == "__main__":
    main()