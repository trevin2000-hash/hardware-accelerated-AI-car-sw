#include <iostream>
#include "ViconPacketReader.h"
#include "vehicle.h"
#include <vector>

using namespace std;

class point
{
public:
    float x, y;


    point(float x1, float y1)
    {
        x = x1;
        y = y1;
    };

    void setP(float x1, float y1)
    {
        x = x1;
        y = y1;
    }
};

int main()
{
    Vehicle car;
    car.update();
    cout << "starting" << endl;
    vector<point> targets;

    point p(2346, 1995);
    targets.push_back(p);

    p.setP(2554, 2420);
    targets.push_back(p);

    p.setP(3020, 2720);
    targets.push_back(p);

    p.setP(3889, 2734);
    targets.push_back(p);

    float targetBearing = 0, error = 0, steer_Control_sig = 0;

    car.setSpeed(50);

    for (auto p = targets.begin(); p != targets.end(); p++)
        while (car.calDis(p->x, p->y) > 50)
        {
            car.update();
            cout << " Dis target" << car.calDis(p->x, p->y) << endl;

            targetBearing = car.calBearing(p->x, p->y);
            error = targetBearing - car.getBearing();

            steer_Control_sig = car.bearingPID(error);

            // cout << "target bearing: " << targetBearing << " CurError: " << error << " Steer Control sig: " << steer_Control_sig << endl;
            car.setStear(steer_Control_sig);
        }

    cout << "exiting program" << endl;
    car.terminate(); // keep the motor from keep moving
    return 0;
}