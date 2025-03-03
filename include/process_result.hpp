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
int find_midpoint(std::vector<std::pair<float, float>> current_lane_line, std::vector<std::pair<float, float>> last_lane_line, cv::Mat img, int img_width)
{
    int x_last, x_current, y_current, x_mid = 0;
    int sum = 0;
    int target_x = 0;
    int target_y = 300;
    int num_valid_points = 0; // some point are negative (this happen when a lane is not point is not detected)

    cout << "left:            mid:          right:" << endl;
    int i; // index of vector of current_lane_line and last_lane_line
    for (; i < current_lane_line.size(); i++)
    {
        x_last = last_lane_line[i].first;        // the first element in the pair is the x value
        x_current = current_lane_line[i].first;  // the first element in the pair is the x value
        y_current = current_lane_line[i].second; // the 2nd element in the pair is the x value
        x_mid = (x_last + x_current) / 2;        // this is the mid point of the road

        if (x_current > 0 && x_last > 0)
        {
            num_valid_points++;
            sum = sum + x_mid;
            cv::Point point(x_mid, y_current);
            cv::circle(img, point, 3, cv::Scalar(0, 255, 255), -1);

            cv::Point point_last(x_last, y_current);
            cv::circle(img, point_last, 3, cv::Scalar(225, 0, 255), -1);

            cv::Point point_currrent(x_current, y_current);
            cv::circle(img, point_currrent, 3, cv::Scalar(255, 0, 255), -1);

            cout << "(" << x_last << "," << y_current << ")" << "            " << "(" << x_mid << "," << y_current << ")" << "           " << "(" << x_current << "," << y_current << ")" << endl;
        }
    }

    target_x = sum / num_valid_points;

    cout << "tagert_x = " << target_x << endl;

    cv::Point point(target_x, 300);
    cv::circle(img, point, 6, cv::Scalar(32, 3, 252), -1);

    return target_x;
}

static cv::Mat process_result(
    cv::Mat &img, int img_width, const vitis::ai::UltraFastResult &result, bool is_jpeg, int &target)
{

    const auto &last_lane_line = result.lanes[1];    // Reference to Lane 1 (avoids copying)
    const auto &current_lane_line = result.lanes[2]; // Reference to Lane 2

    target = find_midpoint(current_lane_line, last_lane_line, img, img_width);

    return img;
}
