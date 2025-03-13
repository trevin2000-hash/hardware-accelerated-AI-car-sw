#include <stdio.h>
#include <stdlib.h>

#include <pigpio.h>

int main(int argc, char *argv[])
{
    int g = 15, f = 100, dc = 500000;
    int status;

    if (argc > 1)
        g = atoi(argv[1]);
    if (argc > 2)
        f = atoi(argv[2]);
    if (argc > 3) 
        dc = atoi(argv[3]);

    printf("gpio %d, f %d, dc %d\n", g, f, dc);

    if (gpioInitialise() < 0)
        return -1;

    status = gpioHardwarePWM(g, f, dc);

    if (status == 0)
    {
        printf("press return to exit\n");
        getchar();
        gpioHardwarePWM(g, 0, 0);
    }

    gpioTerminate();
}