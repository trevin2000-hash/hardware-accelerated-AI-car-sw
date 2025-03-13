// #include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <pigpio.h>
// using namespace std;

int main()
{
    if (gpioInitialise() < 0)
    {
        // pigpio initialisation failed.
        // cout << "pigpio initialisation failed." << endl;
        return 0;
    }

        return 0;
}
