/* A simple server in the internet domain using TCP
The port number is passed as an argument
Based on example: https://www.linuxhowtos.org/C_C++/socket.htm

Modified: Michael Alrøe
Extended to support file server!
*/

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
// #include <string.h>
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

		char filenameBuffer[256]; // Adjust the buffer size as needed. this should be fine
		readTextTCP(newSocketDescriptor, filenameBuffer, sizeof(filenameBuffer));
		printf("Received filename: %s\n", filenameBuffer);

		std::string filename(filenameBuffer); // Convert the filename to a string

		// Check if the file exists
		if (!fileExists(".//datatosend//" + filename))
		{
			printf("File not found: %s\n", filename.c_str());

			// Send a failure message back to the client
			const char *failureMessage = "File not found";
			writeTextTCP(newSocketDescriptor, failureMessage);

			// Close the socket
			close(newSocketDescriptor);

			return; // Exit the function without proceeding to send the file
		}

		// Open the corresponding file for sending based on the received filename.
		file.open(".//datatosend//" + filename, ios::in | ios::binary);

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

	void sendFile()
	{
		char buffer[1024]; // Buffer to read and send file in chunks
		int totalBytesSent = 0;
		int bytesInInterval = 0;

		while (!file.eof())
		{
			// Read a chunk of data from the file
			file.read(buffer, sizeof(buffer));
			int bytesRead = file.gcount();

			if (bytesRead <= 0)
			{
				break; // End of file
			}

			// Send the chunk over the socket
			if (send(newSocketDescriptor, buffer, bytesRead, 0) < 0)
			{
				perror("Error: File not sent");
				exit(1);
			}

			totalBytesSent += bytesRead;
			bytesInInterval += bytesRead;

			// Print progress every 1000 bytes
			if (bytesInInterval >= 1000)
			{
				printf("Sent %d bytes\n", totalBytesSent);
				bytesInInterval = 0; // Reset the interval count
			}
		}

		file.close();
		close(newSocketDescriptor);
		printf("Sent %d bytes\n", totalBytesSent); // Print the final progress
	}
};

int main(int argc, char *argv[]) // probs remove those args
{
	printf("Starting server...\n");
	while (true)
	{
		ServerSocket server;
		server.sendFile();
	}
	return 0;
}