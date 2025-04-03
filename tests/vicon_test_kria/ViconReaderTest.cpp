#include <iostream>
#include "ViconPacketReader.h"
#include <vector>
#include <unistd.h>
using namespace std;

int main()
{
    ViconPacketReader gps_reader;

    while (true)
    {
        auto data_array = gps_reader.read();

        int xPos = data_array[0];
        int yPos = data_array[1];
        int zPos = data_array[2];
        int rotY = data_array[4];
        int rotX = data_array[3];
        int bearing = data_array[5];

        cout << "Current bearing" << bearing << "\nCurrent rot X" << rotX << "\nCurrent y" << rotY;
        cout << "\nXpos = " << xPos << "\nYpos = " << yPos << "\n\n";
    }

    return 0;
}