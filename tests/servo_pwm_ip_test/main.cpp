// main.cpp
#include "pwm_servo_driver.hpp"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    try {
        // Create motor control instance
        motor_control motor;
        
        std::cout << "Motor Control Demo" << std::endl;
        std::cout << "==================" << std::endl;
        
        // Test servo control
        std::cout << "\nTesting servo control..." << std::endl;
        motor.setServoPeriod(1500); // Center position
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        motor.setServoPeriod(1000); // Left position
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        motor.setServoPeriod(2000); // Right position
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        motor.setServoPeriod(1500); // Back to center
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        // Enable motors and test PWM
        std::cout << "\nTesting motors..." << std::endl;
        motor.enableMotors(true);
        
        // Start with low duty cycle
        motor.setBothMotorDuty(50, 50);
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
        // Increase duty cycle
        motor.setBothMotorDuty(100, 100);
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
        // Different duty cycles for each motor (turn)
        motor.setLeftMotorDuty(150);
        motor.setRightMotorDuty(75);
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
        // Stop motors before exiting
        motor.setBothMotorDuty(0, 0);
        motor.enableMotors(false);
        
        std::cout << "\nDemo completed!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}