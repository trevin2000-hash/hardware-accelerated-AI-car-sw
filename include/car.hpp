
#include <opencv2/core.hpp>       // need opencv for video capuring
#include <opencv2/videoio.hpp>    //for video capturing
#include <vitis/ai/ultrafast.hpp> //for ultrafast ai model provide by AI vitis model zoo
#include "udp_handler.hpp"        // for udp_handler class

#ifndef CAR
#define CAR

class Car
{
private:
    cv::VideoCapture cap;                                 // video capture object
    std::unique_ptr<vitis::ai::UltraFast> lanedection_ai; // object for lane detection ai model
    udp_handler esp;                                      // a udp handler the send udp packet for controlling the motor and servo
    int target_x;                                         // target x position of pixel that car need to stay align with to stay in the road
    int delta_x;                                          // how many pixels the camera off from being align with the target
public:
    Car(std::string ip, int port);
    Car(int camera_index, int width, int height, std::string fourcc, std::string model, std::string ip, int port);
    int compute_servo_value(int delta_x);
    void lane_tracking_loop();
};

#endif