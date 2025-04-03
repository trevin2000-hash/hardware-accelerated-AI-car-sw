#include <iostream>
#include <pigpio.h> //for motor and sevro controls
#include <cmath>    //for sqrt for calulatating distacne from target
#include <vector>   //for calulation bearing using vector math
#include <numeric>  //for dot produt
#include <array>
#include "vehicle.h"
#define MOTOR_PIN 4
#define SERVO_PIN 3

using namespace std;

/// @brief construct for Vehicle class, initializes all members and gpio pins
/// @param  void: nothing
Vehicle::Vehicle(void)
{
    this->packet;
    this->xPos = 0;
    this->yPos = 0;
    this->zPos = 0;
    this->bearing = 0;
    this->last_error_b = 0;
    this->inter = 0;

    gpioInitialise();
    gpioSetMode(MOTOR_R, PI_OUTPUT);
    gpioSetMode(SERVO_PIN, PI_OUTPUT);
    gpioSetPWMrange(MOTOR_R, 1000);
};

/// @brief  Must call this function before exit program to cleanly close the gpio threads
/// @param  void: take no args
void Vehicle::terminate(void)
{
    gpioTerminate();
}

/// @brief Updates the Vehicles position values and bearing
/// @param  void: take no args
void Vehicle::update(void)
{
    // data array contains position data, one element per vehicle member
    array<double, 6> data_array = this->packet.read();

    this->xPos = data_array[0];
    this->yPos = data_array[1];
    this->zPos = data_array[2];
    this->rotY = data_array[4];
    this->rotX = data_array[3];
    this->bearing = data_array[5];

    cout << "Current bearing" << this->bearing << " Current rot X" << this->rotX << "Current y" << this->rotY << endl;
    cout << "Xpos = " << this->xPos << "Ypos = " << yPos << endl;
}

/// @brief Sets the speed of the vehicle.
/// @param speed : int value between 0 - 1000. Passing 0 will set the speed to Max speed and  passing 1000 will stop the vehicle.
void Vehicle::setSpeed(int speed)
{
    gpioPWM(MOTOR_R, speed);
}

/// @brief Calculates the distance the vehicle is from a (x,y) point passed to the function
/// @param x2  x position
/// @param y2  y position
/// @return  returns the distance from that position in millimeters
float Vehicle::calDis(double x2, float y2)
{
    // uses pythagorean theorem to calculated the distance from point
    return sqrt(pow(x2 - this->xPos, 2) + pow(y2 - this->yPos, 2));
}

/// @brief sets the steering
/// @param double percentage to turn from the left. 0.00% is fully to the left, 50% is straight, and 100% is fully to the right
void Vehicle::setStear(double percentage)
{
    int servo_value = (710 * (percentage / 100)) + 1050; // servo value needs = 1740 to 1090 or it can damage the servo
    gpioServo(SERVO_PIN, servo_value);
}
/// @brief returns the bearing of the vehicle
/// @return bearing in degrees from North
double Vehicle::getBearing()
{
    return this->bearing;
}

/// @brief PID controller for Bearing
/// @param error
/// @return
float Vehicle::bearingPID(float error)
{
    float kp = 6, ki = 0.0, kd = 0;
    float steering_percentage = 0;

    // this finds your smallest angle to reach your target
    float mag_error = fabs(error);
    if (fabs(error + 360) < mag_error)
    {
        error = error + 360;
    }
    else if (fabs(error - 360) < mag_error)
    {
        error = error - 360;
    }

    this->inter += error * 0.01;

    float derv = (error - this->last_error_b) / 0.01;
    float p = error;
    this->last_error_b = error;

    steering_percentage = -1 * (kp * p + ki * inter + kd * derv) + 50;
    cout << "steer_contr: " << steering_percentage << " = [p] " << kp << "*" << p << " + [i]" << ki << "*" << inter << " +  [d]" << kd << "* " << derv << " + 50" << endl;

    if (steering_percentage > 100)
    {
        steering_percentage = 100;
    }
    else if (steering_percentage < 0)
    {
        steering_percentage = 0;
    }

    return steering_percentage;
}
/// @brief prints X, Y, Z position and bearing from North
void Vehicle::print()
{
    cout << "(" << xPos << ", " << yPos << ", " << zPos << ")<" << bearing << "Degrees" << endl;
}

// this function you vehicle bearing from north need to mantain to reach given (x, y) point
double Vehicle::calBearing(double x2, double y2)
{
    float result = 0;
    vector<double> r = {x2 - xPos, y2 - yPos};
    vector<double> j = {0, 1};

    double mag_r = sqrt(inner_product(r.begin(), r.end(), r.begin(), 0));
    double ry = inner_product(r.begin(), r.end(), j.begin(), 0);

    if (yPos > y2 && xPos > x2)
        result = -180 - (asin(ry / mag_r) * (180 / M_PI));
    else if (yPos < y2 && xPos > x2)
        result = 180 - (asin(ry / mag_r) * (180 / M_PI));
    else
        result = asin(ry / mag_r) * (180 / M_PI);

    return result;
}