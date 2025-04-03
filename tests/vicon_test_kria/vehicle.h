#include "ViconPacketReader.h"

#ifndef VEHICLE
#define VEHICLE

class Vehicle
{
private:
    ViconPacketReader packet;
    double xPos;
    double yPos;
    double zPos;
    double rotX;
    double rotY;
    double bearing;

    // values for PID
    double last_error_b;
    double inter;

public:
    Vehicle(void);
    void terminate(void);
    void update();
    void setSpeed(int speed);
    void setStear(double percentage);
    double getBearing();
    float calDis(double x2, float y2);
    float bearingPID(float error);
    void printData();
    double calBearing(double x2, double y2);
};

#endif