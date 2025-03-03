#ifndef UDP_HANDLER
#define UDP_HANDLER
#include <string>       //need to string ip
#include <sys/socket.h> //for udp socket
#include <arpa/inet.h>  // For inet_pto();
#include <cstring>      //for memset()

class udp_handler
{
private:
    std::string ip;                // ip address of esp
    int port;                      // udp port of esp
    int sockfd;                    // socket file descriptor
    struct sockaddr_in serverAddr; // esp udp socket address

public:
    udp_handler(std::string ip, int port);
    bool send(uint16_t right_duty, uint16_t left_duty, uint16_t servo_period);
    void close_socket();
};

#endif