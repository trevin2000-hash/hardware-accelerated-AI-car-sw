#include <array>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#ifndef VICON_PACKET_READER
#define VICON_PACKET_READER
class ViconPacketReader
{
private:
    int listenfd;
    socklen_t len;
    struct sockaddr_in servaddr;
    struct sockaddr_in cliaddr;

public:
    ViconPacketReader();

    std::array<double, 6> parseData(void *buffer);

    std::array<double, 6> read();
};

#endif