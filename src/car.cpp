#include "car.hpp"

#include <opencv2/core.hpp> // need opencv for video capuring
#include <opencv2/videoio.hpp>
#include <vitis/ai/ultrafast.hpp> //for ultrafast ai model provide by AI vitis model zoo
#include "./process_result.hpp"   //for process_result()

/// @brief default constructor for car class.
/// Uses generic values to set up camera feed pipeline and lane detection AI model
/// @param void: nothing
Car::Car(void)
{

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
    this->lanedection_ai = vitis::ai::UltraFast::create("ultrafast_pt");

    this->target_x = 0;
    this->delta_x = 0;
};

/// @brief constructor for car class. It setup the camera feed pipeline and lane detection AI model
/// @param camera_index : index of camera usb device
/// @param width    : pixel width of the camera feed
/// @param height : pixel height of the camera feed
/// @param fourcc : this is you video codec options (ie: YUYV, MJPG)
/// @param model  : for AI Vitis 2.5: use "ultrafast_pt" or "ultrafast_pt_acc" for model name
Car::Car(int camera_index, int width, int height, std::string fourccStr, std::string model)
{
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

        // Calculate FPS from the time between frames
        auto current_time = std::chrono::steady_clock::now();
        double elapsed_ms = std::chrono::duration<double, std::milli>(current_time - last_time).count();
        double fps = 1000.0 / elapsed_ms; // Convert ms to fps
        last_time = current_time;

        this->delta_x = this->target_x - (this->cap.get(cv::CAP_PROP_FRAME_WIDTH) / 2);

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
    cap.release();
    cv::destroyAllWindows();
}
