#include <fstream>
#include <string>
using namespace std;

int main()
{
    // Replace with the correct serial device on your system:
    const char *portName = "/dev/ttyACM1";

    int duty_r, duty_l = 100;
    int servo = 1500;
    // Open the serial port like a file
    ofstream serialPort(portName);
    if (!serialPort.is_open())
    {
        cout << "Failed to open " << portName << endl;
        return 1;
    }

    while (true)
    {
        serialPort << duty_r << " " << duty_l << " " << servo << " \n";

        cout << "\nR motor duty: ";
        cin >> duty_r;

        cout << "\nL motor duty: ";
        cin >> duty_r;

        cout << "\nservo period (us): ";
        cin >> servo;
    }

    // Close the port
    serialPort.close();
    return 0;
}