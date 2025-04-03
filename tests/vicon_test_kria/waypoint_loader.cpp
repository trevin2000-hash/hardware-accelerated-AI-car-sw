#include "waypoint_loader.hpp"
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>

std::vector<Waypoint> load_waypoints(const std::string &filename)
{
    std::vector<Waypoint> waypoints;
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return waypoints;
    }

    std::string line;
    while (std::getline(file, line))
    {
        // Skip comments or empty lines
        if (line.empty() || line[0] == '#')
        {
            continue;
        }

        std::stringstream ss(line);
        std::string value;
        Waypoint point;

        if (std::getline(ss, value, ','))
        {
            point.x = std::stod(value);
        }
        else
        {
            continue; // Malformed line
        }

        if (std::getline(ss, value, ','))
        {
            point.y = std::stod(value);
        }
        else
        {
            continue; // Malformed line
        }

        waypoints.push_back(point);
    }

    return waypoints;
}
