#include <iostream>
#include <vector>
#include <cmath>
#include "ViconPacketReader.h"
#include "vehicle.h"
#include "CSVReader.h"

using namespace std;

double distance(const Point &a, const Point &b)
{
    return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

int main()
{
    vector<Point> navPoints = load_navigation_points("coordinated.csv");

    // Instantiate the ViconPacketReader
    ViconPacketReader viconReader;

    // Instantiate your vehicle and provide the navigation points
    Vehicle car;
    car.set_navigation_points(navPoints);
}