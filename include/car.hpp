
#include <opencv2/core.hpp> // need opencv for video capuring
#include <opencv2/videoio.hpp>
#include <vitis/ai/ultrafast.hpp> //for ultrafast ai model provide by AI vitis model zoo

#ifndef CAR
#define CAR

class Car
{
private:
    cv::VideoCapture cap;                                 // video capture object
    std::unique_ptr<vitis::ai::UltraFast> lanedection_ai; // object for lane detection ai model
    int target_x;                                         // target x position of pixel that car need to stay align with to stay in the road
    int delta_x;                                          // how many pixels the camera off from being align with the target
public:
    Car();
    Car(int camera_index, int width, int height, std::string fourcc, std::string model);
    void lane_tracking_loop();
    int compute_servo_value(int delta_x);
};

#endif