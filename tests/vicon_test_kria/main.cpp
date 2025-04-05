#include <iostream>
#include "waypoint_loader.hpp"
#include "ViconPacketReader.h"
#include "udp_handler.hpp"        // for udp_handler class
#include <cmath>
#include <string>

int compute_servo_value(int delta_x)
{
    // Proportional gain; adjust this value based on your system's sensitivity.
    const float Kp = 20;
    // Calculate the offset from 1500 using the error (delta_x)
    int offset = Kp * delta_x;
    int servo_value = 1500 - offset;

    std::cout << "Offset: " << offset << std::endl;

    // keep servo value between 1000-2000 to prevent damage to motor
    if (servo_value < 1000)
    {
        servo_value = 1000;
    }
    else if (servo_value > 2000)
    {
        servo_value = 2000;
    }

    return servo_value;
}

void turning_loop()
{
    auto waypoints = load_waypoints("coordinates.csv");

    ViconPacketReader vicon_reader;
    size_t waypoint_index = 0;

    int port = 3333;
    std::string ip = "192.168.100.2";

    udp_handler esp = udp_handler(ip, port);

    while (waypoint_index < waypoints.size())
    {
        auto current_pos = vicon_reader.read();

        double current_x = current_pos[0];
        double current_y = current_pos[1];
        double current_bearing = current_pos[5];

        Waypoint target = waypoints[waypoint_index];

        // Compute differences
        double dx = target.x - current_x;
        double dy = target.y - current_y;
        double distance = sqrt(dx * dx + dy * dy);

        // Check if close enough to target
        if (distance < 50)
        { // threshold for waypoint reach (tune as needed)
            waypoint_index++;
            continue;
        }

        // Compute desired bearing to target point
        double desired_bearing = atan2(dy, dx) * (180.0 / M_PI);
        double bearing_error = desired_bearing - current_bearing;

        // Normalize bearing error to [-180, 180]
        while (bearing_error > 180)
            bearing_error -= 360;
        while (bearing_error < -180)
            bearing_error += 360;

        // Compute servo value based on bearing error
        int servo_value = compute_servo_value(static_cast<int>(bearing_error));

        esp.send(static_cast<uint16_t>(100), static_cast<uint16_t>(100), static_cast<uint16_t>(servo_value));

        std::cout << "Navigating to waypoint " << waypoint_index + 1
                  << "\nServo: " << servo_value
                  << "\nBearing error: " << bearing_error 
                  << "\nDistance: " << distance << std::endl;

        // Small delay or synchronization here (e.g., std::this_thread::sleep_for)
        //std::this_thread::sleep_for(std::chrono::milliseconds(50));

        // Exit turning loop if lane detected again
        //cap >> frame;
        //auto result = lanedection_ai->run(frame);
        //process_result(frame, cap.get(cv::CAP_PROP_FRAME_WIDTH), result, false, this->target_x);
        //if (this->target_x != -1)
        //{
        //    std::cout << "Lane detected again, exiting turning loop.\n";
        //    return;
        //}
    }

    std::cout << "All waypoints traversed." << std::endl;
}

int main()
{

    // Car vehicle("10.0.0.20", 3333);

    //Car vehicle(0, 640, 480, "YUYV", "ultrafast_pt_acc", "192.168.100.2", 3333);

    //vehicle.lane_tracking_loop();
    turning_loop();
 
    return 0;
}
