/*
 * iknlib.c
 *
 * Author: Lars Mortensen
 * Modified: MA
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/stat.h>
#include "iknlib.h"

/**
 * Read a null terminated text stream from a socket
 *
 * @param inSocket socket stream
 * @param maxlength Maximun number of chars for string
 * @param text pointer for char array to hold received text
 */
void readTextTCP(int inSocket, char *text, int maxLength)
{
    char ch = 0;
    int pos = 0;

    while (pos < maxLength - 1)
    {
        if (read(inSocket, &ch, 1) < 0)
        {
            perror("Error reading from socket");
            exit(1);
        }

        if (ch == 0)
            break;

        text[pos++] = ch;
    }

    text[pos] = 0; // Null-terminate the received string
}

/**
 * Writes a null terminated text stream to a socket
 *
 * @param outSocket socket stream
 * @param text Null terminated text to be sent
 */
void writeTextTCP(int outSocket, const char *text)
{
    write(outSocket, text, strlen(text) + 1);
    char nullChar = 0;
    write(outSocket, &nullChar, 1); // Send null termination
}

/**
 * Reads a string holding filesize from a socket, and converts to a number
 *
 * @param inSocket Socket for reading data
 * @return Filesize as a number
 */
long readFileSizeTCP(int inSocket)
{
    char buffer[256] = {0};
    readTextTCP(inSocket, buffer, sizeof(buffer));
    return atol(buffer);
}

/**
 * Extracts a filename from a text string
 *
 * @param text text holding filename and path
 * @return Only filename
 */
const char *extractFileName(const char *text)
{
    char *ecn;
    return ((ecn = (char *)strrchr(text, '/')) == 0 ? text : ++ecn);
}

/**
 * Test for filesize on local disk
 *
 * @param fileName Name of file to test
 * @return Size of file, 0 if file does not exist
 */
long getFilesize(const char *fileName)
{
    struct stat sts;
    if ((stat(fileName, &sts)) == -1)
        return 0;

    return sts.st_size;
}
