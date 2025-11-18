#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Arduino/MCU Communication Test
ทดสอบการสื่อสารกับ Arduino หรือ Microcontroller

สำหรับทดสอบกับ Arduino ให้อัปโหลดโค้ดนี้ลง Arduino ก่อน:

void setup() {
  Serial.begin(115200);  // หรือ baudrate ที่ต้องการทดสอบ
}

void loop() {
  if (Serial.available()) {
    // อ่านข้อมูลและส่งกลับ (echo)
    String data = Serial.readString();
    Serial.print("Echo: ");
    Serial.println(data);
  }
}

Requirements:
pip install pyserial
"""

import serial
import serial.tools.list_ports
import time
import threading

class ArduinoSpeedTest:
    def __init__(self):
        self.ser = None
        self.is_connected = False
        
    def find_arduino_ports(self):
        """หา ports ที่น่าจะเป็น Arduino"""
        ports = serial.tools.list_ports.comports()
        arduino_ports = []
        
        for port in ports:
            # ตรวจหา Arduino จาก description
            desc = port.description.lower()
            if any(keyword in desc for keyword in ['arduino', 'ch340', 'cp210', 'ftdi', 'usb-serial']):
                arduino_ports.append(port)
        
        return arduino_ports
    
    def connect(self, port, baudrate):
        """เชื่อมต่อกับ Arduino"""
        try:
            self.ser = serial.Serial(
                port=port,
                baudrate=baudrate,
                bytesize=serial.EIGHTBITS,
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE,
                timeout=2,
                xonxoff=False,
                rtscts=False,
                dsrdtr=False
            )
            
            # รอ Arduino reset (DTR trigger)
            time.sleep(2)
            
            # ล้าง buffer
            self.ser.reset_input_buffer()
            self.ser.reset_output_buffer()
            
            self.is_connected = True
            return True
            
        except Exception as e:
            print(f"เชื่อมต่อไม่ได้: {e}")
            return False
    
    def test_communication(self):
        """ทดสอบการสื่อสาร echo"""
        if not self.is_connected:
            return False
        
        test_message = "Hello Arduino!"
        try:
            # ส่งข้อความ
            self.ser.write((test_message + '\n').encode())
            
            # รอรับ echo กลับมา
            start_time = time.time()
            while time.time() - start_time < 3:
                if self.ser.in_waiting > 0:
                    response = self.ser.readline().decode().strip()
                    print(f"ส่ง: {test_message}")
                    print(f"รับ: {response}")
                    return "Echo:" in response or test_message in response
                time.sleep(0.1)
            
            print("ไม่ได้รับ response")
            return False
            
        except Exception as e:
            print(f"ข้อผิดพลาด: {e}")
            return False
    
    def throughput_test(self, duration=10, packet_size=64):
        """ทดสอบ throughput"""
        if not self.is_connected:
            return
        
        print(f"\nทดสอบ throughput เป็นเวลา {duration} วินาที...")
        
        # สร้างข้อมูลทดสอบ
        test_data = 'A' * packet_size + '\n'
        
        bytes_sent = 0
        packets_sent = 0
        responses_received = 0
        
        start_time = time.time()
        end_time = start_time + duration
        
        # ส่งข้อมูลต่อเนื่อง
        while time.time() < end_time:
            try:
                # ส่งข้อมูล
                self.ser.write(test_data.encode())
                bytes_sent += len(test_data)
                packets_sent += 1
                
                # ตรวจสอบ response
                if self.ser.in_waiting > 0:
                    response = self.ser.readline()
                    if response:
                        responses_received += 1
                
                time.sleep(0.01)  # หน่วงเล็กน้อย
                
            except Exception as e:
                print(f"ข้อผิดพลาด: {e}")
                break
        
        actual_time = time.time() - start_time
        
        # คำนวณผลลัพธ์
        send_speed = (bytes_sent * 8) / actual_time
        packet_rate = packets_sent / actual_time
        response_rate = (responses_received / packets_sent) * 100 if packets_sent > 0 else 0
        
        print(f"\n--- ผลลัพธ์ ---")
        print(f"เวลาทดสอบ: {actual_time:.2f} วินาที")
        print(f"Packets ส่ง: {packets_sent:,}")
        print(f"Responses รับ: {responses_received:,}")
        print(f"ข้อมูลส่ง: {bytes_sent:,} bytes")
        print(f"ความเร็วส่ง: {send_speed:,.0f} bps ({send_speed/1000:.2f} Kbps)")
        print(f"Packet rate: {packet_rate:.1f} packets/sec")
        print(f"Response rate: {response_rate:.1f}%")
    
    def latency_test(self, num_tests=50):
        """ทดสอบ latency"""
        if not self.is_connected:
            return
        
        print(f"\nทดสอบ latency ({num_tests} ครั้ง)...")
        
        latencies = []
        successful = 0
        
        for i in range(num_tests):
            test_msg = f"PING{i:03d}\n"
            
            try:
                # ส่งและจับเวลา
                start = time.time()
                self.ser.write(test_msg.encode())
                
                # รอ response
                timeout = time.time() + 1  # timeout 1 วินาที
                while time.time() < timeout:
                    if self.ser.in_waiting > 0:
                        response = self.ser.readline()
                        if response:
                            latency = (time.time() - start) * 1000  # แปลงเป็น ms
                            latencies.append(latency)
                            successful += 1
                            break
                    time.sleep(0.001)
                
                time.sleep(0.1)  # หน่วงระหว่างการทดสอบ
                
            except Exception as e:
                print(f"ข้อผิดพลาดในการทดสอบ {i}: {e}")
        
        if latencies:
            avg_latency = sum(latencies) / len(latencies)
            min_latency = min(latencies)
            max_latency = max(latencies)
            
            print(f"\n--- ผลลัพธ์ Latency ---")
            print(f"ทดสอบสำเร็จ: {successful}/{num_tests}")
            print(f"Latency เฉลี่ย: {avg_latency:.2f} ms")
            print(f"Latency ต่ำสุด: {min_latency:.2f} ms")
            print(f"Latency สูงสุด: {max_latency:.2f} ms")
        else:
            print("ไม่สามารถวัด latency ได้")
    
    def disconnect(self):
        """ตัดการเชื่อมต่อ"""
        if self.ser:
            self.ser.close()
            self.is_connected = False

def main():
    print("=== Arduino/MCU Communication Test ===")
    print("หมายเหตุ: Arduino ต้องมีโค้ด echo ที่แสดงในไฟล์นี้")
    
    tester = ArduinoSpeedTest()
    
    # หา Arduino ports
    arduino_ports = tester.find_arduino_ports()
    
    if not arduino_ports:
        # แสดงทุก ports
        all_ports = serial.tools.list_ports.comports()
        print("\nไม่พบ Arduino ports ที่ชัดเจน แสดงทุก ports:")
        for i, port in enumerate(all_ports):
            print(f"{i+1}. {port.device} - {port.description}")
        arduino_ports = all_ports
    else:
        print("\nพบ Arduino ports:")
        for i, port in enumerate(arduino_ports):
            print(f"{i+1}. {port.device} - {port.description}")
    
    if not arduino_ports:
        print("ไม่พบ COM ports")
        return
    
    # เลือก port
    try:
        choice = int(input(f"\nเลือก port (1-{len(arduino_ports)}): ")) - 1
        selected_port = arduino_ports[choice].device
    except:
        print("การเลือกไม่ถูกต้อง")
        return
    
    # เลือก baudrate
    baudrates = [9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600]
    print("\nBaudrates ที่ใช้ทดสอบ:")
    for i, br in enumerate(baudrates):
        print(f"{i+1}. {br}")
    
    try:
        br_choice = int(input(f"เลือก baudrate (1-{len(baudrates)}): ")) - 1
        baudrate = baudrates[br_choice]
    except:
        print("ใช้ baudrate เริ่มต้น 115200")
        baudrate = 115200
    
    # เชื่อมต่อ
    print(f"\nเชื่อมต่อกับ {selected_port} ที่ {baudrate} bps...")
    if not tester.connect(selected_port, baudrate):
        return
    
    print("เชื่อมต่อสำเร็จ!")
    
    # ทดสอบการสื่อสาร
    print("\nทดสอบการสื่อสาร echo...")
    if not tester.test_communication():
        print("การสื่อสารไม่สำเร็จ - ตรวจสอบ Arduino code")
        tester.disconnect()
        return
    
    print("การสื่อสาร echo สำเร็จ!")
    
    # ทดสอบต่างๆ
    try:
        tester.throughput_test(duration=10, packet_size=64)
        tester.latency_test(num_tests=50)
        
    except KeyboardInterrupt:
        print("\nยกเลิกการทดสอบ")
    
    finally:
        tester.disconnect()
        print("\nตัดการเชื่อมต่อแล้ว")

if __name__ == "__main__":
    main()