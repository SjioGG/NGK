/* A simple client in the internet domain using TCP
The ip adresse and port number on server is passed as arguments
Based on example: https://www.linuxhowtos.org/C_C++/socket.htm

Modified: Michael Alrøe
Extended to support file client!
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <sys/socket.h>
#include <netdb.h>
#include "iknlib.h"
#include <arpa/inet.h>

using namespace std;

// #define STRBUFSIZE 256
class ClientSocket
{
	fstream file;
	int port;
	int generalSocketDescriptor;
	struct sockaddr_in serverAddress;
	int addressLength;
	string fileName;

public:
	ClientSocket(const char *ipStr, std::string fileName)
	{
		createSocket();
		port = 9000;
		this->fileName = fileName;
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_port = htons(port);
		addressLength = sizeof(serverAddress);
		if (inet_pton(AF_INET, ipStr, &serverAddress.sin_addr) <= 0)
		{
			perror("ERROR: Invalid address");
			exit(1);
		}

		connectToServer();

		sendFilenameToServer();

		file.open((".//datarecive//") + fileName, ios::out | ios::trunc | ios::binary);
		if (file.is_open())
		{
			printf("File opened\n");
		}
		else
		{
			perror("ERROR: File not opened");
			exit(1);
		}
	}

	void createSocket()
	{
		generalSocketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
		if (generalSocketDescriptor < 0)
		{
			perror("ERROR opening socket");
			exit(1);
		}
		printf("Socket created\n");
	}

	void connectToServer()
	{
		addressLength = sizeof(serverAddress);
		if (connect(generalSocketDescriptor, (struct sockaddr *)&serverAddress, addressLength) < 0)
		{
			perror("ERROR connecting");
			exit(1);
		}
		printf("Connected to server!\n");
	}

	void sendFilenameToServer()
    {
        // Send the filename to the server
        writeTextTCP(generalSocketDescriptor, fileName.c_str());
    }

	void receiveFile()
	{
    	char buffer[1000];
    	int bytesRead;
    	int totalBytesReceived = 0;
    	int bytesInInterval = 0;

    	while ((bytesRead = read(generalSocketDescriptor, buffer, sizeof(buffer))) > 0)
    	{
        	file.write(buffer, bytesRead);
        	totalBytesReceived += bytesRead;
        	bytesInInterval += bytesRead;

        	if (bytesInInterval >= 1000)
        	{
            	printf("Received %d bytes\n", totalBytesReceived);
            	bytesInInterval = 0; // Reset the interval count
        	}
    	}

    	if (bytesRead < 0)
    	{
        	perror("ERROR reading from socket");
        	exit(1);
    	}
		
		printf("Received %d bytes\n", totalBytesReceived); // Print the final progress
 		printf("File received\n");
    	file.close();
	}
};

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		std::cerr << "Usage: " << argv[0] << " <IP address> <filename>" << std::endl;
		return 1;
	}

	const char *ipStr = argv[1];
	const char *filename = argv[2];

	// Convert the IP address from string to binary form and store it in serverAddress

	std::cout << "Starting client with IP address: " << ipStr << " and filename: " << filename << std::endl;
	// Now you can use the 'serverAddress' struct and 'filename' to create the ClientSocket.
	ClientSocket clientSocket(ipStr, filename);
	clientSocket.receiveFile();

	return 0;
}
