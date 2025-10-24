#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <Arduino.h>
#include "system.h"

// Motor PWM control limits
#define MOTOR_PWM_MIN        0          // Minimum PWM value
#define MOTOR_PWM_MAX        4095       // Maximum PWM value  
#define MOTOR_PWM_DEFAULT    3000       // Default PWM value

// Global variables
extern bool flag_motorRunning;               // Motor running state

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

/* @brief Stop the motor operation
 * 
 * @param error Error flag indicating if stop is due to an error (default: false)
 */
void stopMotor();          

/* @brief Test motor control functionality
*/
void testMotorControl();                    // Test motor control functionality
#endif
