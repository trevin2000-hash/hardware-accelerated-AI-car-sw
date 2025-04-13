// pwm_servo_driver.hpp
#ifndef PWM_SERVO_DRIVER_HPP
#define PWM_SERVO_DRIVER_HPP

#include <cstdint>

// Define base addresses for the IP cores
#define PWM_LEFT_ADDR 0xb0000000
#define PWM_RIGHT_ADDR 0xb0010000
#define SERVO_ADDR 0xb0020000

// Register offsets
#define PWM_DUTY_REG_OFFSET 0x0     // Offset for duty cycle register (reg 0)
#define PWM_ENABLE_REG_OFFSET 0x4   // Offset for enable register (reg 1)
#define SERVO_PERIOD_REG_OFFSET 0x0 // Offset for servo period register (reg 0)

class motor_control
{
private:
    int fd;              // File descriptor for /dev/mem
    void *pwm_left_map;  // Memory map for left PWM
    void *pwm_right_map; // Memory map for right PWM
    void *servo_map;     // Memory map for servo

    // Helper function to write to a register
    void writeRegister(void *base_addr, uint32_t offset, uint32_t value);

    // Helper function to read from a register
    uint32_t readRegister(void *base_addr, uint32_t offset);

public:
    // Constructor - initializes memory mapping
    motor_control();

    // Destructor - cleans up memory mapping and disables motors
    ~motor_control();

    // Enable/disable motors
    void enableMotors(bool enable);

    // Set duty cycle for left motor (0-255)
    void setLeftMotorDuty(uint8_t duty);

    // Set duty cycle for right motor (0-255)
    void setRightMotorDuty(uint8_t duty);

    // Set both motor duty cycles at once
    void setBothMotorDuty(uint8_t left_duty, uint8_t right_duty);

    // Set servo period (1000-2000 μs)
    void setServoPeriod(uint16_t period_us);
};

#endif