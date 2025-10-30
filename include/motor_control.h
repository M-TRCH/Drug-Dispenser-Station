#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <Arduino.h>
#include "system.h"

// Motor PWM control limits
#define MOTOR_PWM_MIN        0          // Minimum PWM value
#define MOTOR_PWM_MAX        4095       // Maximum PWM value  
#define MOTOR_PWM_DEFAULT    3000       // Default PWM value
#define MOTOR_PWM_SLOW       1500       // Slow speed for precise positioning
#define MOTOR_PWM_DECEL      1000       // Deceleration speed

// Motor control modes
enum MotorMode {
    MOTOR_STOP = 0,
    MOTOR_FORWARD = 1,
    MOTOR_REVERSE = 2
};

// Global variables
extern bool flag_motorRunning;               // Motor running state
extern MotorMode currentMotorMode;           // Current motor direction
extern int currentMotorSpeed;                // Current motor speed

/* @brief Initialize motor control system
 * 
 * This function sets up the motor control pins and ensures the motor
 * is in a stopped state at initialization.
 */
void motorControlInit();                    

/* @brief Start the motor with specified speed and direction
 * 
 * @param pwmSpeed PWM speed value (default: MOTOR_PWM_DEFAULT)
 * @param forward Direction flag (true for forward, false for reverse; default: true)
 */
void startMotor(int pwmSpeed=MOTOR_PWM_DEFAULT, bool forward=true);

/* @brief Stop the motor operation with optional braking
 * 
 * @param brake Enable dynamic braking (default: false)
 */
void stopMotor(bool brake=false);          

/* @brief Gradually change motor speed (for smooth acceleration/deceleration)
 * 
 * @param targetSpeed Target PWM speed
 * @param stepSize Speed change per step (default: 100)
 * @param stepDelay Delay between steps in ms (default: 10)
 */
void changeMotorSpeed(int targetSpeed, int stepSize=100, int stepDelay=10);

/* @brief Get current motor status
 * 
 * @return Motor status string
 */
String getMotorStatus();

/* @brief Test motor control functionality
*/
void testMotorControl();                    // Test motor control functionality

#endif
