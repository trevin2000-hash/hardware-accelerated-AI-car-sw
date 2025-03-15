#include "CSVReader.h"
#include <fstream>
#include <sstream>
#include <iostream>

std::vector<Point> loadNavigationPoints(const std::string &filename)
{
    std::vector<Point> points;
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return points;
    }

    std::string line;
    while (std::getline(file, line))
    {
        // Skip empty or comment lines.
        if (line.empty() || line[0] == '#')
            continue;

        std::istringstream linestream(line);
        std::string token;
        std::vector<std::string> tokens;
        while (std::getline(linestream, token, ','))
        {
            tokens.push_back(token);
        }

        if (tokens.size() < 2)
            continue;

        try
        {
            Point p;
            p.x = std::stoi(tokens[0]);
            p.y = std::stoi(tokens[1]);
            points.push_back(p);
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error converting coordinates in line: " << line << "\n"
                      << "Exception: " << e.what() << std::endl;
        }
    }
    return points;
}
