#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <pigpio.h>
#include <unistd.h> //for usleep()
using namespace std;
#define MOTOR_PIN 4
#define SERVO_PIN 3

int main()
{
    int freq = 4000;
    int duty_cycle = 500;
    int servovalue = 1500;

    gpioCfgClock(10, 0, 0); // this allow the pwm clock to be set to 20khz
    gpioInitialise();

    gpioSetMode(MOTOR_PIN, PI_OUTPUT);
    gpioSetMode(SERVO_PIN, PI_OUTPUT);

    gpioSetPWMrange(MOTOR_PIN, 1000);
    // gpioServo(SERVO_PIN, 2500);

    gpioPWM(MOTOR_PIN, 1000);
    // gpioWrite(MOTOR_PIN, 1);
    gpioSetPWMfrequency(MOTOR_PIN, freq);

    cout << "\n\bFrenquency (hz): " << gpioGetPWMfrequency(MOTOR_PIN) << "\n";

    while (true)
    {
        gpioPWM(MOTOR_PIN, duty_cycle);
        gpioServo(SERVO_PIN, servovalue);

        // for (servovalue = 1100, duty_cycle = 1000; servovalue <= 1900; servovalue += 10, duty_cycle -= 0)
        // {
        //     cout << "servo value: " << servovalue << "Motor duty" << (duty_cycle) << endl;
        //     gpioServo(SERVO_PIN, servovalue);
        //     gpioPWM(MOTOR_PIN, duty_cycle);
        //     usleep(8000);
        // }
        // usleep(500000);
        // for (servovalue = 1900, duty_cycle = 0; servovalue >= 1100; servovalue -= 10, duty_cycle += 0)
        // {
        //     cout << "servo value: " << servovalue << "Motor duty" << (duty_cycle) << endl;
        //     gpioServo(SERVO_PIN, servovalue);
        //     gpioPWM(MOTOR_PIN, duty_cycle);
        //     usleep(8000);
        // }

        cout << "Servo pwm value: ";
        cin >> servovalue;

        // cout << " current duty cycle" << duty_cycle
        //      << "Enter PWM value for motor:";
        // cin >> duty_cycle;
    }
    gpioTerminate();
    return 0;
}