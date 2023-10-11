#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: ./get_measurement <ip> U (or) L" << std::endl;
        exit(1);
    }

    const char *serverIP = argv[1];
    char command = argv[2][0];

    int clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket == -1)
    {
        std::cerr << "Error creating socket" << std::endl;
        exit(1);
    }

    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(9000);
    inet_pton(AF_INET, serverIP, &serverAddress.sin_addr);

    sendto(clientSocket, &command, 1, 0, (struct sockaddr *)&serverAddress, sizeof(serverAddress));

    char response[1024];
    socklen_t serverAddressLength = sizeof(serverAddress);

    ssize_t receivedBytes = recvfrom(clientSocket, response, sizeof(response), 0, (struct sockaddr *)&serverAddress, &serverAddressLength);
    if (receivedBytes == -1) {
        std::cerr << "Error receiving data" << std::endl;
        close(clientSocket);
        exit(1);
    }

    response[receivedBytes] = '\0';
    std::cout << "Server response: " << response << std::endl;

    close(clientSocket);
    return 0;
}
