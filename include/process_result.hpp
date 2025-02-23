/*
 * Copyright 2019 Xilinx Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once
#include <sys/stat.h>

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>
#include "vitis/ai/ultrafast.hpp"

using namespace cv;
using namespace std;

Scalar colors[] = {Scalar(255, 0, 0), Scalar(0, 255, 0), Scalar(255, 255, 0), Scalar(0, 0, 255)};

// Draw midpoints between adjacent lanes
int find_midpoint(std::vector<std::pair<float, float>> current_lane_line, std::vector<std::pair<float, float>> last_lane_line, cv::Mat img)
{
    int x_last, x_current, y_current, x_mid_point = 0;
    int target_x = 0;
    int i; // index of vector of current_lane_line and last_lane_line
    for (; i < current_lane_line.size(); i++)
    {
        x_last = last_lane_line[i].first;        // the first element in the pair is the x value
        x_current = current_lane_line[i].first;  // the first element in the pair is the x value
        y_current = current_lane_line[i].second; // the 2nd element in the pair is the x value
        x_mid_point = (x_last + x_current) / 2;  // this is the mid point of the road

        if (x_current > 0 && x_last > 0)
        {
            if (i == current_lane_line.size() / 2)
            {
                target_x = x_mid_point;
                cv::Point point(target_x, y_current);
                cv::circle(img, point, 3, cv::Scalar(255, 0, 0), -1);
            }
            // else
            // {
            //     cv::Point point(x_mid_point, y_current);
            //     cv::circle(img, point, 3, cv::Scalar(0, 255, 255), -1);
            // }
        }
    }
    return target_x;
}

// void add_lane_points(std::vector<std::pair<float, float>> lane_points, int lane_line_count, cv::Mat img)
// {
//     for (auto &x_and_y_pos : lane_points)
//     {
//         if (x_and_y_pos.first > 0)
//         { // check is the x pos is not a negative number
//             cv::Point current_lane_line(x_and_y_pos.first, x_and_y_pos.second);
//             cv::circle(img, current_lane_line, 3, colors[lane_line_count % 4], -1);
//         }
//     }
// }

static cv::Mat process_result(
    cv::Mat &img, int img_width, const vitis::ai::UltraFastResult &result, bool is_jpeg, int &target)
{

    // int lane_line_count = 0;
    // std::vector<std::pair<float, float>> last_lane_line; // need to record lane line to calculate center of the road
    // vector<vector<cv::Point>> lane_points;               // Store lane points for later midpoint calculations

    // std::vector<std::pair<float, float>> last_lane_line = result.lanes[1];    // Lane 1
    // std::vector<std::pair<float, float>> current_lane_line = result.lanes[2]; // Lane 2
    const auto &last_lane_line = result.lanes[1];    // Reference to Lane 1 (avoids copying)
    const auto &current_lane_line = result.lanes[2]; // Reference to Lane 2

    target = find_midpoint(current_lane_line, last_lane_line, img);

    // for (auto &current_lane_line : result.lanes)
    // {
    //     std::vector<cv::Point> current_lane;
    //     std::cout << "lane: " << lane_line_count << "\n";

    //     if (lane_line_count == 1 || lane_line_count == 2) // only process lane line 1 and 2
    //     {
    //         if (lane_line_count == 2)
    //         {
    //             target = find_midpoint(current_lane_line, last_lane_line, img);
    //         }

    //         // add_lane_points(current_lane_line, lane_line_count, img);
    //     }

    //     last_lane_line = current_lane_line;
    //     lane_line_count++;
    // }

    return img;
}
