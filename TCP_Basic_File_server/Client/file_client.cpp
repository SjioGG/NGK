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
	ClientSocket(struct sockaddr_in serverAddress, std::string fileName)
	{
		createSocket();
		port = 9000;
		this->serverAddress = serverAddress;
		this->fileName = fileName;
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_port = htons(port);
		addressLength = sizeof(serverAddress);
		if (inet_pton(AF_INET, serverAddress, &serverAddress.sin_addr) <= 0)
		{
			perror("ERROR: Invalid address");
			exit(1);
		}

		connectToServer();
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
		if (connect(generalSocketDescriptor, (struct sockaddr *)&serverAddress, adressLength) < 0)
		{
			perror("ERROR connecting");
			exit(1);
		}
		printf("Connected to server!\n");
	}

	void receiveFile()
	{
		char buffer[1024] = {};
		int valread = read(generalSocketDescriptor, buffer, 1024);
		printf("Receiving, size: %d\n" valread);
		file << buffer;
		printf("File received\n");
	}
};

int main(string argv[])
{
	printf("Starting client...\n");
	ClientSocket clientSocket(argv[1], argv[2]);
	clientSocket.receiveFile();
	return 0;
}
