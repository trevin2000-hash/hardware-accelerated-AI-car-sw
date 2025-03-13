#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <pigpio.h>
#include <unistd.h> //for usleep()
using namespace std;
#define ENCODER_R 15
#define MOTOR_R 3 
#define MOTOR_L 4

int main() {

    gpioInitialise();
    gpioSetMode(ENCODER_R, PI_INPUT);
    gpioSetMode(MOTOR_R, PI_OUTPUT);

    // gpioSetPullUpDown(ENCODER_PIN, PI_PUD_OFF);

    gpioPWM(MOTOR_R, 3);

    int tick = 0;
    while(1){ 
        // cout << "GPIO LEVEL: " << gpioRead(ENCODER_R) << endl;
        if(gpioRead(ENCODER_R) == PI_HIGH){
            tick++;
            cout << "Tick = " << tick << endl;
            while(gpioRead(ENCODER_R) == PI_HIGH){
                //wait
            } 
        }
    }

    gpioTerminate();
}
