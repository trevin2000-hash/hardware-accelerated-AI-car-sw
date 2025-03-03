#include <iostream>
#include <string>
#include <cstring>
#include "udp_handler.hpp"

int main()
{
    std::string ip = "10.0.0.20";
    udp_handler esp = udp_handler(ip, 3333);
    int rightDuty, leftDuty, servoUs;

    while (true)
    {
        std::cout << "enter right duty:" << std::endl;
        std::cin >> rightDuty;

        std::cout << "enter left duty:" << std::endl;
        std::cin >> leftDuty;

        std::cout << "enter servo value:" << std::endl;
        std::cin >> servoUs;

        bool succ = esp.send(static_cast<uint16_t>(rightDuty), static_cast<uint16_t>(leftDuty), static_cast<uint16_t>(servoUs));

        if (!succ)
        {
            std::cout << "failed to send" << rightDuty << " " << leftDuty << " " << servoUs << std::endl;
        }
        std::cout << "success to send" << rightDuty << " " << leftDuty << " " << servoUs << std::endl;
    }

    esp.close_socket();
    return 0;
}