#ifndef WAYPOINT_LOADER_H
#define WAYPOINT_LOADER_H

#include <vector>
#include <string>

struct Waypoint
{
    double x, y;
};

std::vector<Waypoint> load_waypoints(const std::string &filename);

#endif