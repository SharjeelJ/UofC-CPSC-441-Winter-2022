/* This file contains modified code from the following source(s):
 * https://www.binarytides.com/socket-programming-c-linux-tutorial/ (Socket Programming in C on Linux provided by the Professor under Useful Links on the course page)
 * Client.cpp from T02/T03 Week1
 * Server.cpp from T02/T03 Week1
*/

#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

// Global static variables
#define IP  "136.159.5.25"
#define PORT    44144
#define MESSAGE "bob"

// Main function
int main() {
    // Creates a struct that will store the complete address of the socket being created
    struct sockaddr_in socketAddress{};

    // Allocates memory to the location that will store the address of the socket
    memset(&socketAddress, 0, sizeof(socketAddress));

    // Sets the socket address family to IPv4
    socketAddress.sin_family = AF_INET;

    // Sets the port number that the socket will use
    socketAddress.sin_port = htons(PORT);

    // Converts the IPv4 address of the socket to binary otherwise prints an error and returns if unsuccessful
    int addressValidity = inet_pton(AF_INET, IP, &socketAddress.sin_addr);
    if (addressValidity == 0) {
        printf("IPv6 Address Provided (Require IPv4 Address)!\n");
        return -1;
    } else if (addressValidity == -1) {
        printf("Invalid Address (Require IPv4 Address)!\n");
        return -1;
    }

    // Creates a socket that will be used to communicate with the server otherwise prints an error and returns if unsuccessful
    int customSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (customSocket == -1) {
        printf("Socket Creation Failed!\n");
        return -1;
    }

    // Establishes a connection using the socket otherwise prints an error and returns if unsuccessful
    int connectionStatus = connect(customSocket, (struct sockaddr *) &socketAddress, sizeof(struct sockaddr_in));
    if (connectionStatus == -1) {
        printf("Socket Connection Failed!\n");
        return -1;
    }

    // Sends the message using the socket otherwise prints an error and returns if unsuccessful
    char message[100] = {MESSAGE};
    int bytesSent = send(customSocket, message, 5, 0);
    if (bytesSent == -1) {
        printf("Socket Send Failed!\n");
        return -1;
    }

    // Receives the reply using the socket otherwise prints an error and returns if unsuccessful
    char reply[2000];
    int bytesReceived = recv(customSocket, reply, 2000, 0);
    if (bytesReceived == -1) {
        printf("Socket Receive Failed!\n");
        return -1;
    }

    // Prints out the original message followed by the server's reply
    printf("%s : %s", message, reply);

    // Closes the socket
    close(customSocket);

    // Ends the program with the success exit code
    return 0;
}
