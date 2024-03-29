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
		// createSocket();
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

		// Open the file to write to
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

	// void requestFileFromServer() // THIS IS WITH C-IMPL, which brakes evertyhing
	// {
	// 	// Send the filename to the server as a request
	// 	writeTextTCP(generalSocketDescriptor, fileName.c_str());

	// 	// Receive a response from the server
	// 	char response[256];
	// 	readTextTCP(generalSocketDescriptor, response, sizeof(response));
	// 	// Check the response to see if the file is available
	char failed[256] = "Requested file does not exist";
	// 	if (strcmp(response, failed) == 0)
	// 	{
	//     	printf("File not available or unexpected response from server: %s\n", response);
	//     	exit(1);
	// 	}
	// 	else
	// 	{
	// 		printf("File found!: %s\n", response);
	// 	}
	// 	//writeTextTCP(generalSocketDescriptor, fileName.c_str());
	// }

	void requestFileFromServer() // Fix with send and recv, but problem with progress print
	{
		// Send the filename to the server as a request
		writeTextTCP(generalSocketDescriptor, fileName.c_str());
		char response[256];
		int bytesRead = recv(generalSocketDescriptor, response, sizeof(response), 0);
		if (strcmp(response, failed) == 0)
		{
			printf("File not available or unexpected response from server: %s\n", response);
			exit(1);
		}
		else
		{
			printf("File found!: %s\n", response);
		}
	}

	void receiveFile()
	{
		// Receive the file size from the client
		int fileSizeNetworkOrder;
		if (recv(generalSocketDescriptor, &fileSizeNetworkOrder, sizeof(fileSizeNetworkOrder), 0) < 0)
		{
			perror("Error: File size not received");
			exit(1);
		}
		int fileSize = ntohl(fileSizeNetworkOrder);

		char buffer[1000];
		int totalBytesReceived = 0;
		int bytesInInterval = 0;
		int bytesRead;

		while (totalBytesReceived < fileSize)
		{
			bytesRead = recv(generalSocketDescriptor, buffer, sizeof(buffer), 0);

			if (bytesRead < 0)
			{
				perror("ERROR reading from socket");
				exit(1);
			}

			// Write to the file
			file.write(buffer, bytesRead);

			totalBytesReceived += bytesRead;
			bytesInInterval += bytesRead;

			// Print progress every 1000 bytes
			if (bytesInInterval >= 1000)
			{
				printf("Received %d bytes (%.2f%%)\n", totalBytesReceived, (static_cast<double>(totalBytesReceived) / fileSize) * 100);
				bytesInInterval = 0; // Reset the interval count
			}
		}

		printf("Received %d bytes (100.00%%)\n", totalBytesReceived); // Print the final progress
		file.close();
		close(generalSocketDescriptor);
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

	std::cout << "Starting client with IP address: " << ipStr << " and filename: " << filename << std::endl;
	// Now you can use the 'serverAddress' struct and 'filename' to create the ClientSocket.
	ClientSocket clientSocket(ipStr, filename);
	clientSocket.createSocket();
	clientSocket.connectToServer();
	clientSocket.requestFileFromServer();
	clientSocket.receiveFile();

	return 0;
}
