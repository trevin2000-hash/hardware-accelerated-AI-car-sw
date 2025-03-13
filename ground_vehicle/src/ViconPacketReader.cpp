#include <iostream>
#include "ViconPacketReader.h"
#include <array>  //c++ arry for read()
#include <math.h> //for M_PI

// libs for udp packets
#include <strings.h> //for bzero
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
using namespace std;

#define MAX_BUF_SIZE 1024
#define PORT 5002

ViconPacketReader::ViconPacketReader()
{
    bzero(&servaddr, sizeof(servaddr));

    // Create a UDP Socket
    cliaddr.sin_addr.s_addr = inet_addr("10.213.1.34");
    cliaddr.sin_port = htons(5001);
    cliaddr.sin_family = AF_INET;

    listenfd = socket(AF_INET, SOCK_DGRAM, 0);
    servaddr.sin_addr.s_addr = htons(INADDR_ANY);
    servaddr.sin_port = htons(PORT);
    servaddr.sin_family = AF_INET;

    // bind server address to socket descriptor
    bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    cout << "sent bytes" << sendto(listenfd, "hello", sizeof("hello"), 0, (sockaddr *)&cliaddr, sizeof(cliaddr));
    len = sizeof(cliaddr);
}

std::array<double, 6> ViconPacketReader::parseData(void *buffer)
{
    array<double, 6> data;
    void *tempVP = buffer; // copy pointer address
    tempVP += 4;           // skip the first 4 bytes of packet header

    int8_t numItems = *((int8_t *)tempVP); // number of objects in the data packet to be read
    // cout << "numItems = " << numItems << endl;
    tempVP += 4;

    tempVP += 24; // skip 24 bytes of string data

    data[0] = *((double *)tempVP);
    tempVP += 8;

    data[1] = *((double *)tempVP);
    tempVP += 8;

    data[2] = *((double *)tempVP);
    tempVP += 8;

    data[3] = (*((double *)tempVP)) * (180 / M_PI);
    tempVP += 8;

    data[4] = (*((double *)tempVP)) * (180 / M_PI);
    tempVP += 8;

    data[5] = (*((double *)tempVP)) * (180 / M_PI);
    tempVP += 8;

    return data;
}

/// For printing err for udp packet,
/// called in read()
void print_err()
{
    switch (errno)
    {
    case EWOULDBLOCK:
        cout << "The socket's file descriptor is marked O_NONBLOCK and no data is waiting to be received; or MSG_OOB is set and no out-of-band data is available and either the socket's file descriptor is marked O_NONBLOCK or the socket does not support blocking to await out-of-band data" << endl;
        break;
    case EBADF:
        cout << "The socket argument is not a valid file descriptor" << endl;
        break;
    case ECONNRESET:
        cout << "A connection was forcibly closed by a peer" << endl;
        break;
    case EFAULT:
        cout << "The buffer parameter can not be accessed or written" << endl;
        break;
    case EINTR:
        cout << "The recv" << endl;
        break;
    case EINVAL:
        cout << "The MSG_OOB flag is set and no out-of-band data is available" << endl;
        break;
    case ENOTCONN:
        cout << "A receive is attempted on a connection-mode socket that is not connected" << endl;
        break;
    case ENOTSOCK:
        cout << "The socket argument does not refer to a socket" << endl;
        break;
    case EOPNOTSUPP:
        cout << "The specified flags are not supported for this socket type or protocol" << endl;
        break;
    case ETIMEDOUT:
        cout << "The connection timed out during connection establishment, or due to a transmission timeout on active connection" << endl;
        break;
    case EIO:
        cout << "An I/O error occurred while reading from or writing to the file system" << endl;
        break;
    case ENOBUFS:
        cout << "Insufficient resources were available in the system to perform the operation" << endl;
        break;
    case ENOMEM:
        cout << "Insufficient memory was available to fulfill the request" << endl;
        break;
    case ENOSR:
        cout << "There were insufficient STREAMS resources available for the operation to complete" << endl;
        break;
    }
}

std::array<double, 6> ViconPacketReader::read()
{

    void *buffer = malloc(MAX_BUF_SIZE);

    int n = recvfrom(this->listenfd, buffer, MAX_BUF_SIZE, 0, (struct sockaddr *)&cliaddr, &len); // receive message from server
    if (n == -1)
    {
        print_err();
    }

    return parseData(buffer);
}
