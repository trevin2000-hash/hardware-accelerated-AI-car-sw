// main.cpp
#include "pwm_servo_driver.hpp"
#include <iostream>
#include <string>
#include <limits>

int main()
{
    std::cout << "Simple Motor Control Test Program" << std::endl;

    try
    {
        // Create the motor control object
        motor_control controller;

        // Ask user if they want to enable motors
        int enable;
        std::cout << "Enable motors? (0=Off, 1=On): ";
        std::cin >> enable;
        controller.enableMotors(enable == 1);

        // Main control loop
        bool running = true;
        while (running)
        {
            int left_duty, right_duty, servo_period;

            // Get left motor duty
            std::cout << "\nEnter left motor duty (0-255, -1 to exit): ";
            std::cin >> left_duty;
            if (left_duty < 0)
            {
                break; // Exit if negative value entered
            }
            if (left_duty > 255)
                left_duty = 255;

            // Get right motor duty
            std::cout << "Enter right motor duty (0-255): ";
            std::cin >> right_duty;
            if (right_duty < 0)
                right_duty = 0;
            if (right_duty > 255)
                right_duty = 255;

            // Get servo period
            std::cout << "Enter servo period (1000-2000 μs): ";
            std::cin >> servo_period;
            if (servo_period < 1000)
                servo_period = 1000;
            if (servo_period > 2000)
                servo_period = 2000;

            // Send values to hardware
            controller.setBothMotorDuty(left_duty, right_duty);
            controller.setServoPeriod(servo_period);

            // Clear any input errors
            if (std::cin.fail())
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        }

        // Disable motors before exiting
        controller.enableMotors(false);
        std::cout << "Program exiting..." << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}