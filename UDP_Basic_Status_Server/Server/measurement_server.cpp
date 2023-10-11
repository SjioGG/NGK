#include <iostream>
#include <cstring>
#include <cstdlib>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>
#include <string>

int main()
{
    int serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket == -1)
    {
        std::cerr << "Error creating socket" << std::endl;
        exit(1);
    }

    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(9000);

    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        std::cerr << "Error binding socket" << std::endl;
        close(serverSocket);
        exit(1);
    }

    while (true)
    {
        char buffer[1]; // To receive 'U' or 'L'
        struct sockaddr_in clientAddress;
        socklen_t clientAddressLength = sizeof(clientAddress);

        ssize_t receivedBytes = recvfrom(serverSocket, buffer, sizeof(buffer), 0,
                                         (struct sockaddr *)&clientAddress, &clientAddressLength);
        if (receivedBytes == -1)
        {
            std::cerr << "Error receiving data" << std::endl;
            close(serverSocket);
            exit(1);
        }

        if (buffer[0] == 'U' || buffer[0] == 'u')
        {
            // Read and send /proc/uptime to the client
            std::ifstream uptimeFile("/proc/uptime");
            std::string uptime;
            getline(uptimeFile, uptime);
            sendto(serverSocket, uptime.c_str(), uptime.length(), 0, (struct sockaddr *)&clientAddress, clientAddressLength);
        }
        else if (buffer[0] == 'L' || buffer[0] == 'l')
        {
            // Read and send /proc/loadavg to the client
            std::ifstream loadavgFile("/proc/loadavg");
            std::string loadavg;
            getline(loadavgFile, loadavg);
            sendto(serverSocket, loadavg.c_str(), loadavg.length(), 0, (struct sockaddr *)&clientAddress, clientAddressLength);
        }
    }

    close(serverSocket);
    return 0;
}
