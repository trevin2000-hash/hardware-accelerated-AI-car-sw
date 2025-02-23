#include <glog/logging.h>
#include <iostream>
#include <memory>
#include <string> // for std::stoi
#include <chrono> // for high_resolution_clock or steady_clock
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <vitis/ai/ultrafast.hpp>
#include "./process_result.hpp"

int main(int argc, char *argv[])
{
    int target_x, delta_x = 0;
    // Check if the camera index is provided
    int camera_index = 0; // Default to the first USB camera
    int width = 640;
    int height = 480;
    std::string fourccStr = "YUYV";
    int fourcc = cv::VideoWriter::fourcc(
        fourccStr[0],
        fourccStr[1],
        fourccStr[2],
        fourccStr[3]);

    if (argc > 1)
    {
        camera_index = std::stoi(argv[1]);
    }

    // Open the USB camera
    cv::VideoCapture cap(camera_index, cv::CAP_V4L2);
    if (!cap.isOpened())
    {
        std::cerr << "Error: Could not open USB camera." << std::endl;
        return -1;
    }

    // Optional: Set resolution if provided as additional arguments
    if (argc > 3)
    {
        width = std::stoi(argv[2]);
        height = std::stoi(argv[3]);
        std::cout << "Camera resolution: " << width << "x" << height << std::endl;
        cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
        cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);
        cap.set(cv::CAP_PROP_FOURCC, fourcc);
    }
    else
    {
        // Default resolution
        cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
        cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);
        cap.set(cv::CAP_PROP_FOURCC, fourcc);
    }

    // Create the Ultra-Fast Lane Detection model
    std::string model = "ultrafast_pt"; // Update with your actual model name if needed
    auto ultrafast = vitis::ai::UltraFast::create(model);

    // For calculating FPS based on time per loop
    auto last_time = std::chrono::steady_clock::now();

    while (true)
    {
        cv::Mat frame;
        cap >> frame; // Capture a new frame
        if (frame.empty())
        {
            std::cerr << "Error: Blank frame grabbed from USB camera." << std::endl;
            break;
        }

        // Run the Ultra-Fast model
        auto result = ultrafast->run(frame);

        // Process the result
        cv::Mat processed_frame = process_result(frame, target_x, result, false, target_x);

        // Calculate FPS from the time between frames
        auto current_time = std::chrono::steady_clock::now();
        double elapsed_ms = std::chrono::duration<double, std::milli>(current_time - last_time).count();
        double fps = 1000.0 / elapsed_ms; // Convert ms to fps
        last_time = current_time;

        // Display the FPS on the processed image
        cv::putText(processed_frame,
                    "FPS: " + std::to_string(fps) + " delta x: " + std::to_string(target_x - (width / 2)),
                    cv::Point(10, 30),
                    cv::FONT_HERSHEY_SIMPLEX,
                    1.0,
                    cv::Scalar(0, 255, 0),
                    2);

        // Show the processed frame
        cv::imshow("Ultra-Fast Lane Detection", processed_frame);

        // Press 'q' to exit
        if (cv::waitKey(1) == 'q')
        {
            break;
        }
    }

    // Release the camera and close OpenCV windows
    cap.release();
    cv::destroyAllWindows();

    return 0;
}
