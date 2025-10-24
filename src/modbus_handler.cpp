// #include "modbus_handler.h"
// // #include "motor_control.h"
// // #include "system.h"

// ModbusRTUServerClass RTUServer;

// void setupModbus() {
//     // Initialize Modbus RTU server
//     if (!RTUServer.begin(rs485, MODBUS_SLAVE_ID, RS485_BAUDRATE, SERIAL_8N1)) {
//         Serial.println("[Modbus] ERROR: Failed to start RTU Server!");
//         return;
//     }

//     // Configure holding registers
//     RTUServer.configureHoldingRegisters(0, MODBUS_REGISTER_COUNT);

//     // Initialize register values
//     RTUServer.holdingRegisterWrite(REG_RUN, 0);      // Stop command
//     RTUServer.holdingRegisterWrite(REG_SPEED, MOTOR_PWM_DEFAULT);  // Default speed
//     RTUServer.holdingRegisterWrite(REG_STATUS, 0);   // Stopped status
//     RTUServer.holdingRegisterWrite(REG_COUNT, 0);    // Zero count
//     RTUServer.holdingRegisterWrite(REG_TARGET, DISPENSE_TARGET_DEFAULT);  // Default target
//     RTUServer.holdingRegisterWrite(REG_RESET, 0);    // No reset

//     Serial.printf("[Modbus] RTU Server started (Slave ID: %d)\n", MODBUS_SLAVE_ID);
// }

// void handleModbus() {
//     // Process Modbus communication
//     if (RTUServer.poll()) {
//         // Read command registers
//         int runCmd = RTUServer.holdingRegisterRead(REG_RUN);
//         int speed = RTUServer.holdingRegisterRead(REG_SPEED);
//         int target = RTUServer.holdingRegisterRead(REG_TARGET);
//         int resetCmd = RTUServer.holdingRegisterRead(REG_RESET);

//         // Handle reset command
//         if (resetCmd == 1) {
//             resetCounter();
//             RTUServer.holdingRegisterWrite(REG_RESET, 0);  // Clear reset flag
//         }

//         // Handle run/stop commands
//         if (runCmd == 1) {
//             // Start motor with specified speed
//             int motorSpeed = (speed > 0) ? speed : MOTOR_PWM_DEFAULT;
//             startMotor(motorSpeed, true);
//             dispenseTarget = target;
//             RTUServer.holdingRegisterWrite(REG_STATUS, 1);  // Running status
//         } else {
//             // Stop motor
//             stopMotor(false);
//             RTUServer.holdingRegisterWrite(REG_STATUS, 0);  // Stopped status
//         }

//         // Auto-stop when target reached
//         if (motorRunning && objectCount >= dispenseTarget) {
//             stopMotor(false);
//             RTUServer.holdingRegisterWrite(REG_RUN, 0);     // Clear run command
//             RTUServer.holdingRegisterWrite(REG_STATUS, 0);  // Stopped status
//         }
//     }

//     // Update count register continuously
//     RTUServer.holdingRegisterWrite(REG_COUNT, objectCount & 0xFFFF);
// }
