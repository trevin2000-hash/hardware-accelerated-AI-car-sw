#include <iostream>
#include "car.hpp"

int main()
{

    // Car vehicle("10.0.0.20", 3333);

    Car vehicle(0, 640, 480, "YUYV", "ultrafast_pt_acc", "192.168.100.2", 3333);

    vehicle.lane_tracking_loop();

    return 0;
}