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

// #define STRBUFSIZE 256
class ClientSocket
{
	fstream file;
	int port;
	int generalSocketDescriptor;
	struct sockaddr_in serverAddress;
	int adressLength;

public:
	ClientSocket(int port, struct sockaddr_in serverAddress, fstream file)
	{
		createSocket();
		this->port = port;
		this->serverAddress = serverAddress;
		this->file = file;
	}

	void createSocket()
	{
		generalSocketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
		if (generalSocketDescriptor < 0)
		{
			error("ERROR opening socket");
		}
	}

	void connectToServer()
	{
		adressLength = sizeof(serverAddress);
		if (connect(generalSocketDescriptor, (struct sockaddr *)&serverAddress, adressLength) < 0)
		{
			error("ERROR connecting");
		}
	}

	void receiveFile(int serverSocket, const char *fileName, long fileSize)
	{
		printf("Receiving: '%s', size: %li\n", fileName, fileSize);
	}
}

int
main(int argc, char *argv[])
{
	printf("Starting client...\n");
	ClientSocket clientSocket(arg...);
	if (argc < 3)
	{
		error("ERROR usage: "
			  "hostname"
			  ",  "
			  "filename"
			  "\n");
	}

	return 0;
}
