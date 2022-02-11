/* This file contains modified code from the following source(s):
 * https://www.binarytides.com/socket-programming-c-linux-tutorial/ (Socket Programming in C on Linux provided by the Professor under Useful Links on the course page)
 * http://pages.cpsc.ucalgary.ca/~carey/CPSC441/ass0/mypal-server.c (Professor's palindrome checking server)
*/

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>

// Global static variables
#define MAX_MESSAGE_SIZE 1024
#define PORT 48259

// Creates a socket that will be used by the fork to allow the client to communicate with the proxy
int clientSocketTCP;

// Main function
int main() {
    // Creates a struct that will store the complete address of the socket being created
    struct sockaddr_in socketAddress = {};
    struct sockaddr_in clientAddress = {};

    // Creates char arrays that will store the message between the client and the server
    char tcpCompleteMessage[MAX_MESSAGE_SIZE];
    char tcpSplitMessage[MAX_MESSAGE_SIZE];
    char udpSplitMessage[MAX_MESSAGE_SIZE];

    // Initialize int variables to store the number of bytes each char array holds
    int tcpCompleteMessageBytes = 0;
    int tcpSplitMessageBytes = 0;
    int udpSplitMessageBytes = 0;

    // Initializes the incoming and outgoing message and temporary buffer arrays with zeroed bytes
    memset(tcpCompleteMessage, 0, MAX_MESSAGE_SIZE);
    memset(tcpSplitMessage, 0, MAX_MESSAGE_SIZE);
    memset(udpSplitMessage, 0, MAX_MESSAGE_SIZE);

    // Creates a socket that will be used by the client to initially communicate with the proxy
    int serverSocketTCP;
    int serverSocketUDP;

    // Will be used to store the PID of a child fork
    pid_t pid;

    // Allocates memory to the location that will store the address of the socket
    memset(&socketAddress, 0, sizeof(socketAddress));
    memset(&clientAddress, 0, sizeof(clientAddress));

    // Sets the socket address family to IPv4
    socketAddress.sin_family = AF_INET;

    // Sets the port number that the socket will use
    socketAddress.sin_port = htons(PORT);

    // Sets the complete IP address that the socket will use
    socketAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    // Creates a socket that will be used to communicate between the server and the client otherwise prints an error and returns if unsuccessful
    serverSocketTCP = socket(PF_INET, SOCK_STREAM, 0);
    if (serverSocketTCP == -1) {
        fprintf(stderr, "Server: TCP socket() failed!\n");
        exit(1);
    }

    // Creates a socket that will be used to communicate between the server and the client otherwise prints an error and returns if unsuccessful
    serverSocketUDP = socket(PF_INET, SOCK_DGRAM, 0);
    if (serverSocketUDP == -1) {
        fprintf(stderr, "Server: UDP socket() failed!\n");
        exit(1);
    }

    // Binds the specific address and port number to the socket otherwise prints an error and returns if unsuccessful
    if (bind(serverSocketTCP, (struct sockaddr *) &socketAddress, sizeof(struct sockaddr_in)) == -1) {
        fprintf(stderr, "Server: TCP bind() failed!\n");
        exit(1);
    }

    // Binds the specific address and port number to the socket otherwise prints an error and returns if unsuccessful
    if (bind(serverSocketUDP, (struct sockaddr *) &socketAddress, sizeof(struct sockaddr_in)) == -1) {
        fprintf(stderr, "Server: UDP bind() failed!\n");
        exit(1);
    }

    // Listens on the socket for an incoming client connection
    if (listen(serverSocketTCP, 5) == -1) {
        fprintf(stderr, "Server: TCP listen() failed!\n");
        exit(1);
    }

    // Prints out the welcome message and TCP & UDP port number that the proxy is operating on
    fprintf(stderr, "Server running on TCP & UDP port %d...\n\n", PORT);

    // Loops forever to handle client requests
    while (true) {
        // Accepts an incoming socket connection request from the client
        clientSocketTCP = accept(serverSocketTCP, NULL, NULL);
        if (clientSocketTCP == -1) {
            fprintf(stderr, "Server: TCP accept() failed!\n");
            exit(1);
        }

        // Creates a fork that will deal with the client
        pid = fork();

        // Only proceeds further if the fork creation was successful (0 is returned)
        if (pid < 0) {
            fprintf(stderr, "Server: fork() failed!\n");
            exit(1);
        } else if (pid == 0) {
            // Closes the inherited socket from the parent as the child does not need it
            close(serverSocketTCP);

            // Receives the client's message using the socket
            bzero(tcpCompleteMessage, MAX_MESSAGE_SIZE);
            tcpCompleteMessageBytes = recv(clientSocketTCP, tcpCompleteMessage, MAX_MESSAGE_SIZE, 0);

            while (tcpCompleteMessageBytes > 0) {
                // Sends the server's reply to the client
                sprintf(tcpSplitMessage, "TCP REPLY");
                tcpSplitMessageBytes = strlen(tcpSplitMessage);
                if (send(clientSocketTCP, tcpSplitMessage, tcpSplitMessageBytes, 0) < 0) {
                    fprintf(stderr, "Server: TCP send() failed!\n");
                    exit(1);
                }

                int len = sizeof(clientAddress);
                char test[MAX_MESSAGE_SIZE];
                recvfrom(serverSocketUDP, test, MAX_MESSAGE_SIZE,
                         MSG_WAITALL, (struct sockaddr *) &clientAddress,
                         (socklen_t *) &len);

                // Sends the server's reply to the client
                sprintf(udpSplitMessage, "UDP REPLY");
                udpSplitMessageBytes = strlen(udpSplitMessage);
                if (sendto(serverSocketUDP, udpSplitMessage, udpSplitMessageBytes,
                           MSG_CONFIRM, (const struct sockaddr *) &clientAddress,
                           len) < 0) {
                    fprintf(stderr, "Server: UDP send() failed!\n");
                    exit(1);
                }

                bzero(tcpCompleteMessage, MAX_MESSAGE_SIZE);
                bzero(tcpSplitMessage, MAX_MESSAGE_SIZE);
                bzero(udpSplitMessage, MAX_MESSAGE_SIZE);

                tcpCompleteMessageBytes = recv(clientSocketTCP, tcpCompleteMessage, MAX_MESSAGE_SIZE, 0);
            }

            // Closes the socket with the client and kills the fork
            fprintf(stderr, "Child process received nothing, so it is exiting now\n");
            close(clientSocketTCP);
            exit(0);
        } else {
            // Informs the user of the child's PID (that will be handling the client and proxy connection) and that the proxy server will now continue to listen for new requests
            fprintf(stderr, "Proxy Server created child process %d to handle the client\n", pid);
            fprintf(stderr, "Proxy Server process going back to listening for new clients now...\n\n");

            // Parent does not need the socket that communicates with the client
            close(clientSocketTCP);
        }
    }
}
