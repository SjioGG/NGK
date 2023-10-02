/* A simple server in the internet domain using TCP
The port number is passed as an argument
Based on example: https://www.linuxhowtos.org/C_C++/socket.htm

Modified: Michael Alrøe
Extended to support file server!
*/

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "iknlib.h"

using namespace std;
// #define STRBUFSIZE 256
class ServerSocket
{
	fstream file;
	int port;
	int generalSocketDescriptor;
	int newSocketDescriptor;
	struct sockaddr_in serverAddress;
	int adressLength;

public:
	ServerSocket()
	{
		createSocket(); // implement it
		port = 9000;
		serverAddress.sin_family = AF_INET;			// IPv4
		serverAddress.sin_addr.s_addr = INADDR_ANY; // any address
		serverAddress.sin_port = htons(port);		// host to network short
		adressLength = sizeof(serverAddress);		// set length of address

		bindSocket();
		listenSocket();
		acceptSocket();

		file.open(".//datatosend//img.jpg", ios::in | ios::binary); // probably shorten this down, either one or the other
		if (!file.is_open())
		{
			perror("Error: File not opened");
			exit(1);
		}
		else
		{
			printf("File opened\n");
		}
	}
	void createSocket()
	{
		if ((generalSocketDescriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		{
			perror("Error: Socket not created");
			exit(1);
		}
		else
		{
			printf("Socket created\n");
		}
	}
	void bindSocket()
	{
		if (bind(generalSocketDescriptor, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
		{
			perror("Error: Socket not binded");
			exit(1);
		}
		else
		{
			printf("Socket binded\n");
		}
	}
	void listenSocket()
	{
		if (listen(generalSocketDescriptor, 5) < 0)
		{
			perror("Error: Socket not listening");
			exit(1);
		}
		else
		{
			printf("Socket listening\n");
		}
	}
	void acceptSocket()
	{
		if ((newSocketDescriptor = accept(generalSocketDescriptor, (struct sockaddr *)&serverAddress, (socklen_t *)&adressLength)) < 0)
		{
			perror("Error: Socket not accepted");
			exit(1);
		}
		else
		{
			printf("Socket accepted\n");
		}
	}

	void sendFile()
	{
		std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		printf("Transmission Data : %s Bytes", contents.c_str());
		if (send(newSocketDescriptor, contents.c_str(), contents.length(), 0) < 0)
		{
			perror("Error: File not sent");
			exit(1);
		}
		else
		{
			printf("File sent\n");
		}
	}
};

int main(int argc, char *argv[]) // probs remove those args
{
	printf("Starting server...\n");
	ServerSocket server;
	server.sendFile();
	return 0;
}