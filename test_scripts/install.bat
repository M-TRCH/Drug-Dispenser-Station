@echo off
echo =======================================
echo   RS232 Speed Test Tools - Setup
echo =======================================

echo.
echo Installing required Python packages...
pip install pyserial

echo.
echo Installation completed!
echo.
echo Available tools:
echo   1. python rs232_speed_test.py      - Complete speed test
echo   2. python simple_rs232_test.py     - Simple speed test  
echo   3. python arduino_comm_test.py     - Arduino communication test
echo   4. python quick_test.py COM3 115200 - Quick test
echo.
echo See README_RS232_TEST.md for detailed instructions
echo.
pause