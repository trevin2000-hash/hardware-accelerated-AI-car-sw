#include "car.hpp"
#include "waypoint_loader.hpp"
#include "ViconPacketReader.h"

#include <opencv2/core.hpp> // need opencv for video capuring
#include <opencv2/videoio.hpp>
#include <vitis/ai/ultrafast.hpp> //for ultrafast ai model provide by AI vitis model zoo
#include <thread>                 //for imshow()
#include "./process_result.hpp"   //for process_result()
#include "udp_handler.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief default constructor for car class.
/// Uses generic values to set up camera feed pipeline and lane detection AI model
/// @param ip the ip address of the esp8266
/// @param port the socket of the esp8266
Car::Car(std::string ip, int port)
{

    this->esp = udp_handler(ip, port);

    // set up video capture device
    int camera_index = 0;           // index of camera usb device
    int width = 640;                // pixel width of the camera feed
    int height = 480;               // pixel height of the camera feed
    std::string fourccStr = "YUYV"; // raw video codec
    int fourcc = cv::VideoWriter::fourcc(
        fourccStr[0],
        fourccStr[1],
        fourccStr[2],
        fourccStr[3]);

    this->cap = cv::VideoCapture(camera_index, cv::CAP_V4L2);
    if (!cap.isOpened())
    {
        std::cerr << "ERROR: could not open USB camera device \n";
    }

    this->cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
    this->cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);
    this->cap.set(cv::CAP_PROP_FOURCC, fourcc);

    // create the ultra-Fast Lane detection model
    this->lanedection_ai = vitis::ai::UltraFast::create("ultrafast_pt_acc");

    this->target_x = 0;
    this->delta_x = 0;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief constructor for car class. It setup the camera feed pipeline and lane detection AI model
/// @param camera_index : index of camera usb device
/// @param width    : pixel width of the camera feed
/// @param height : pixel height of the camera feed
/// @param fourcc : this is you video codec options (ie: YUYV, MJPG)
/// @param model  : for AI Vitis 2.5: use "ultrafast_pt" or "ultrafast_pt_acc" for model name
/// @param ip the ip address of the esp8266
/// @param port the socket of the esp8266
Car::Car(int camera_index, int width, int height, std::string fourccStr, std::string model, std::string ip, int port)
{
    this->esp = udp_handler(ip, port);

    // set up video capture device
    int fourcc = cv::VideoWriter::fourcc(
        fourccStr[0],
        fourccStr[1],
        fourccStr[2],
        fourccStr[3]);

    this->cap = cv::VideoCapture(camera_index, cv::CAP_V4L2);
    if (!cap.isOpened())
    {
        std::cerr << "ERROR: could not open USB camera device \n";
    }

    this->cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
    this->cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);
    this->cap.set(cv::CAP_PROP_FOURCC, fourcc);

    // create the ultra-Fast Lane detection model
    this->lanedection_ai = vitis::ai::UltraFast::create(model);

    this->target_x = 0;
    this->delta_x = 0;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief This use a proportional controller to calculate the need servo value to stay in the lane.
/// @param delta_x The number of horizontal pixel away the center of camera image is from the center of lane
/// @return The servo value that is need to stay inside the lane
int Car::compute_servo_value(int delta_x)
{
    // Proportional gain; adjust this value based on your system's sensitivity.
    const float Kp = 6;
    // Calculate the offset from 1500 using the error (delta_x)
    int offset = Kp * delta_x;
    int servo_value = 1500 + offset;

    if (servo_value < 0) // if negative  set it to zero
    {
        servo_value = 0;
    }

    return servo_value;
}

void Car::turning_loop()
{
    auto waypoints = load_waypoints("coordinates.csv");

    ViconPacketReader vicon_reader;
    size_t waypoint_index = 0;

    cv::Mat frame; 

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
        if (distance < 10)
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
        //int servo_value = compute_servo_value(static_cast<int>(bearing_error));

        //esp.send(static_cast<uint16_t>(100), static_cast<uint16_t>(100), static_cast<uint16_t>(servo_value));

        std::cout << "Navigating to waypoint " << waypoint_index + 1
        //          << " Servo: " << servo_value
                  << " Bearing error: " << bearing_error << std::endl;

        // Small delay or synchronization here (e.g., std::this_thread::sleep_for)
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief This function loops through: grabs a frame from the camera, send it to AI, then process result returned from AI.
/// the while loop does stop until you 'q'.
void Car::lane_tracking_loop()
{

    auto last_time = std::chrono::steady_clock::now(); // For calculating FPS based on time per loop
    cv::Mat frame;                                     // frame capture from camera for each loop
    cv::Mat process_frame;                             // frame that has points marked on it of where the lanes are
    vitis::ai::UltraFastResult result;                 // result from AI model after processing a frame

    while (true)
    {
        cap >> frame;
        if (frame.empty())
        {
            std::cerr << "Error: Blank frame grabbed from USB camera." << std::endl;
            break;
        }

        // Run the lane detection AI model
        result = this->lanedection_ai->run(frame);

        // Process result
        process_frame = process_result(frame,
                                       this->cap.get(cv::CAP_PROP_FRAME_WIDTH),
                                       result,
                                       false,
                                       this->target_x);


        if (this->target_x == -1)
        {
            turning_loop();
        }

        // Calculate FPS from the time between frames
        auto current_time = std::chrono::steady_clock::now();
        double elapsed_ms = std::chrono::duration<double, std::milli>(current_time - last_time).count();
        double fps = 1000.0 / elapsed_ms; // Convert ms to fps
        last_time = current_time;

        // Compute servo value using proportional controller
        this->delta_x = this->target_x - (this->cap.get(cv::CAP_PROP_FRAME_WIDTH) / 2);
        int servo_value = compute_servo_value(this->delta_x);
        std::cout << "Servo Value: " << servo_value << std::endl;

        // send servo value to motor controller
        esp.send(static_cast<uint16_t>(100), static_cast<uint16_t>(100), static_cast<uint16_t>(servo_value));

        // Display the FPS on the processed image
        cv::putText(process_frame,
                    "FPS: " + std::to_string(fps) + " delta x: " + std::to_string(this->delta_x),
                    cv::Point(10, 30),
                    cv::FONT_HERSHEY_SIMPLEX,
                    1.0,
                    cv::Scalar(0, 255, 0),
                    2);

        // Show the processed frame
        cv::imshow("Ultra-Fast Lane Detection", process_frame);

        // Press 'q' to exit
        if (cv::waitKey(1) == 'q')
        {
            break;
        }
    }

    // Release the camera and close OpenCV windows
    this->cap.release();
    this->esp.close_socket();
    cv::destroyAllWindows();
}