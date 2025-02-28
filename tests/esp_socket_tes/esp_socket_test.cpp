#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

// Configuration constants
const char *ESP_IP_ADDRESS = "10.0.0.24"; // Replace with your ESP8266's IP address
const int ESP_UDP_PORT = 3333;

// Function to send command to ESP8266
bool sendUdpCommand(int rightDuty, int leftDuty, int servoUs)
{
    // Create UDP socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "Error opening socket" << std::endl;
        return false;
    }

    // Set up the server address structure
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(ESP_UDP_PORT);

    // Convert IP address from string to binary form
    if (inet_pton(AF_INET, ESP_IP_ADDRESS, &serverAddr.sin_addr) <= 0)
    {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        close(sockfd);
        return false;
    }

    // Constrain values to valid ranges
    rightDuty = (rightDuty < 0) ? 0 : (rightDuty > 255) ? 255
                                                        : rightDuty;
    leftDuty = (leftDuty < 0) ? 0 : (leftDuty > 255) ? 255
                                                     : leftDuty;
    servoUs = (servoUs < 1000) ? 1000 : (servoUs > 2500) ? 2500
                                                         : servoUs;

    // Format the command string
    std::stringstream ss;
    ss << rightDuty << " " << leftDuty << " " << servoUs;
    std::string command = ss.str();

    // Send the message
    if (sendto(sockfd, command.c_str(), command.length(), 0,
               (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        std::cerr << "Failed to send data" << std::endl;
        close(sockfd);
        return false;
    }

    std::cout << "Sent command: " << command << std::endl;

    // Close the socket
    close(sockfd);
    return true;
}

int main()
{
    int rightDuty, leftDuty, servoUs;

    while (true)
    {
        std::cout << "Enter values (rightDuty leftDuty servoUs) or 'q' to quit: ";
        std::string input;
        std::getline(std::cin, input);

        if (input == "q" || input == "quit")
        {
            break;
        }

        std::stringstream ss(input);
        if (ss >> rightDuty >> leftDuty >> servoUs)
        {
            if (sendUdpCommand(rightDuty, leftDuty, servoUs))
            {
                std::cout << "Command sent successfully" << std::endl;
            }
            else
            {
                std::cout << "Failed to send command" << std::endl;
            }
        }
        else
        {
            std::cout << "Invalid input. Please enter three integers." << std::endl;
        }
    }

    return 0;
}