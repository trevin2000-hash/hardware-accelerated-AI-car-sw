#ifndef CSV_READER_H
#define CSV_READER_H

#include <vector>
#include <string>

// Simple Point structure to hold x,y coordinates.
struct Point
{
    int x;
    int y;
};

// Loads navigation points from a CSV file. Lines starting with '#' are ignored.
std::vector<Point> loadNavigationPoints(const std::string &filename);

#endif // CSV_READER_H
