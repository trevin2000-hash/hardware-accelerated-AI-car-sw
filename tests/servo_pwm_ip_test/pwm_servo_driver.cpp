// pwm_servo_driver.cpp
#include "pwm_servo_driver.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <iostream>
#include <cstring>
#include <cerrno>
#include <stdexcept>

// Size of memory map for each device (a single page is enough)
#define MAP_SIZE 4096

motor_control::motor_control() {
    // Open /dev/mem for physical memory access
    fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) {
        std::cerr << "Error opening /dev/mem: " << strerror(errno) << std::endl;
        throw std::runtime_error("Failed to open /dev/mem");
    }

    // Map memory regions for each IP core
    pwm_left_map = mmap(nullptr, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, PWM_LEFT_ADDR);
    if (pwm_left_map == MAP_FAILED) {
        close(fd);
        std::cerr << "Error mapping left PWM memory: " << strerror(errno) << std::endl;
        throw std::runtime_error("Failed to map memory for left PWM");
    }

    pwm_right_map = mmap(nullptr, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, PWM_RIGHT_ADDR);
    if (pwm_right_map == MAP_FAILED) {
        munmap(pwm_left_map, MAP_SIZE);
        close(fd);
        std::cerr << "Error mapping right PWM memory: " << strerror(errno) << std::endl;
        throw std::runtime_error("Failed to map memory for right PWM");
    }

    servo_map = mmap(nullptr, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, SERVO_ADDR);
    if (servo_map == MAP_FAILED) {
        munmap(pwm_left_map, MAP_SIZE);
        munmap(pwm_right_map, MAP_SIZE);
        close(fd);
        std::cerr << "Error mapping servo memory: " << strerror(errno) << std::endl;
        throw std::runtime_error("Failed to map memory for servo");
    }

    // Initialize by disabling motors
    enableMotors(false);
    
    std::cout << "Robot control initialized successfully" << std::endl;
}

motor_control::~motor_control() {
    // Disable motors before cleanup
    enableMotors(false);
    
    // Unmap memory regions
    munmap(pwm_left_map, MAP_SIZE);
    munmap(pwm_right_map, MAP_SIZE);
    munmap(servo_map, MAP_SIZE);
    
    // Close file descriptor
    close(fd);
    
    std::cout << "Robot control cleaned up" << std::endl;
}

void motor_control::writeRegister(void* base_addr, uint32_t offset, uint32_t value) {
    *((volatile uint32_t*)((uint8_t*)base_addr + offset)) = value;
}

uint32_t motor_control::readRegister(void* base_addr, uint32_t offset) {
    return *((volatile uint32_t*)((uint8_t*)base_addr + offset));
}

void motor_control::enableMotors(bool enable) {
    uint32_t value = enable ? 1 : 0;
    writeRegister(pwm_left_map, PWM_ENABLE_REG_OFFSET, value);
    writeRegister(pwm_right_map, PWM_ENABLE_REG_OFFSET, value);
    std::cout << "Motors " << (enable ? "enabled" : "disabled") << std::endl;
}

void motor_control::setLeftMotorDuty(uint8_t duty) {
    writeRegister(pwm_left_map, PWM_DUTY_REG_OFFSET, duty);
    std::cout << "Left motor duty set to " << (int)duty << std::endl;
}

void motor_control::setRightMotorDuty(uint8_t duty) {
    writeRegister(pwm_right_map, PWM_DUTY_REG_OFFSET, duty);
    std::cout << "Right motor duty set to " << (int)duty << std::endl;
}

void motor_control::setBothMotorDuty(uint8_t left_duty, uint8_t right_duty) {
    setLeftMotorDuty(left_duty);
    setRightMotorDuty(right_duty);
}

void motor_control::setServoPeriod(uint16_t period_us) {
    // Ensure period is within valid range (1000-2000 μs)
    if (period_us < 1000) period_us = 1000;
    if (period_us > 2000) period_us = 2000;
    
    writeRegister(servo_map, SERVO_PERIOD_REG_OFFSET, period_us);
    std::cout << "Servo period set to " << period_us << " μs" << std::endl;
}