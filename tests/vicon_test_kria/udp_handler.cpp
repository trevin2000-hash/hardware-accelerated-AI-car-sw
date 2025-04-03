#include <string>       // Need to string ip
#include <sys/socket.h> // For udp socket
#include <arpa/inet.h>  // For inet_pto();
#include <iostream>     // For cerr and cout
#include <cstring>      // For memset()
#include <unistd.h>     // For close()

#include "udp_handler.hpp"

/// @brief creates udp socket for sending right and left duty motors duty and servo period for the esp8266
/// @param ip the ip address of the esp8266
/// @param port the socket of the esp8266
udp_handler::udp_handler(std::string ip, int port)
{
    this->ip = ip;
    this->port = port;
    this->sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (this->sockfd < 0)
    {
        std::cerr << "Error opening socket" << std::endl;
    }

    // Set up the server address structure

    memset(&this->serverAddr, 0, sizeof(this->serverAddr));
    this->serverAddr.sin_family = AF_INET;
    this->serverAddr.sin_port = htons(port);

    if (inet_pton(AF_INET, this->ip.c_str(), &this->serverAddr.sin_addr) <= 0)
    {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        close(sockfd);
    }
}

/// @brief the UDP packet containing the left right motor duty
///        and servo period to the esp8266
/// @param right_duty value between 0 - 225. Zero is the max speed and
///                   225 is full stop
/// @param left_duty value between 0 - 225. Zero is the max speed and
///                  225 is full stop
/// @param servo_period unit of microseconds. value between 1000
/// @return True when sends values successfully
bool udp_handler::send(uint16_t right_duty, uint16_t left_duty, uint16_t servo_period)
{

    // Constrain values to valid ranges
    // the right and left motors must be contraint to 0 to 225
    // the servo period must be contraint to 1000 us to 2000 us
    // right_duty = (right_duty < (uint16_t)0) ? 0 : ((uint16_t)right_duty > 255) ? 255
    //                                                                          : right_duty;
    // left_duty = (left_duty < (uint16_t)0) ? 0 : (left_duty > (uint16_t)255) ? 255
    //                                                                       : left_duty;
    // servo_period = (servo_period < (uint16_t)1000) ? 1000 : (servo_period > (uint16_t)2500) ? 2500
    //                                                                                       : servo_period;

    // Prepare binary packet (6 bytes total)
    uint8_t buffer[6];

    // Store each uint16_t in network byte order (big-endian)
    buffer[0] = (right_duty >> 8) & 0xFF; // High byte
    buffer[1] = right_duty & 0xFF;        // Low byte

    buffer[2] = (left_duty >> 8) & 0xFF; // High byte
    buffer[3] = left_duty & 0xFF;        // Low byte

    buffer[4] = (servo_period >> 8) & 0xFF; // High byte
    buffer[5] = servo_period & 0xFF;        // Low byte

    // Send the binary packet
    if (sendto(sockfd, buffer, sizeof(buffer), 0,
               (struct sockaddr *)&(this->serverAddr), sizeof(this->serverAddr)) < 0)
    {
        std::cerr << "Failed to send data" << std::endl;
        close(this->sockfd);
        return false;
    }

    std::cout << "Sent binary command: "
              << "Right=" << right_duty
              << ", Left=" << left_duty
              << ", Servo=" << servo_period << std::endl;
    return true;
}

/// @brief closes the file sock file descriptor.
/// needs to be before closing the program
void udp_handler::close_socket()
{
    close(this->sockfd);
}
