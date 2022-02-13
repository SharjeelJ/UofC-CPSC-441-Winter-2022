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
#define MAX_MESSAGE_SIZE 2048
#define PORT 48259
#define DEVOWEL_VALUE 1
#define ENVOWEL_VALUE 2

// 0 = Simple split/merge, 1 = Advanced split/merge, 2 = Custom split/merge
#define MODE 0

// Creates a socket that will be used by the fork to allow the client to communicate with the server
int clientSocketTCP;

// Main function
int main() {
    // Creates a struct that will store the complete address of the server
    struct sockaddr_in serverAddress;

    // Creates a struct that will store the complete address of the client
    struct sockaddr *clientAddress;

    // Creates char arrays that will store the message between the client and the server
    char tcpIncomingMessage[MAX_MESSAGE_SIZE];
    char tcpOutgoingMessage[MAX_MESSAGE_SIZE];
    char udpMessage[MAX_MESSAGE_SIZE];

    // Initialize int variables to store the number of bytes each char array holds
    int tcpIncomingMessageBytes = 0;
    int tcpOutgoingMessageBytes = 0;
    int udpMessageBytes = 0;

    // Initializes the incoming and outgoing message and temporary buffer arrays with zeroed bytes
    memset(tcpIncomingMessage, 0, MAX_MESSAGE_SIZE);
    memset(tcpOutgoingMessage, 0, MAX_MESSAGE_SIZE);
    memset(udpMessage, 0, MAX_MESSAGE_SIZE);

    // Creates a socket that will be used by the client to initially communicate with the proxy
    int serverSocketTCP;
    int serverSocketUDP;

    // Will be used to store the PID of a child fork
    pid_t pid;

    // Allocates memory to the location that will store the addresses
    memset(&serverAddress, 0, sizeof(serverAddress));
    memset(&clientAddress, 0, sizeof(clientAddress));

    // Sets the socket address family to IPv4
    serverAddress.sin_family = AF_INET;

    // Sets the port number that the socket will use
    serverAddress.sin_port = htons(PORT);

    // Sets the complete IP address that the socket will use
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);

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

    // Sets the timeout duration for the UDP socket when receiving messages
    struct timeval timeoutDuration = {10, 0};
    setsockopt(serverSocketUDP, SOL_SOCKET, SO_RCVTIMEO, (const char *) &timeoutDuration, sizeof timeoutDuration);

    // Binds the specific address and port number to the socket otherwise prints an error and returns if unsuccessful
    if (bind(serverSocketTCP, (struct sockaddr *) &serverAddress, sizeof(struct sockaddr_in)) == -1) {
        fprintf(stderr, "Server: TCP bind() failed!\n");
        exit(1);
    }

    // Binds the specific address and port number to the socket otherwise prints an error and returns if unsuccessful
    if (bind(serverSocketUDP, (struct sockaddr *) &serverAddress, sizeof(struct sockaddr_in)) == -1) {
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
        clientSocketTCP = accept(serverSocketTCP, clientAddress, NULL);
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

            // Gets the information sent by the client and stores the client's UDP port number
            char clientInfo[MAX_MESSAGE_SIZE];
            if (recv(clientSocketTCP, clientInfo, MAX_MESSAGE_SIZE, 0) < 0) {
                printf("Server <-> Client Configuration Communication Failed!\n");
                exit(-1);
            }
            int udpPortClient = atoi(clientInfo);

            // Stores the complete client UDP address with the provided port
            sockaddr_in udpClientAddress = reinterpret_cast<const sockaddr_in &>(clientAddress);
            udpClientAddress.sin_port = htons(udpPortClient);

            // Receives the client's message using the socket
            bzero(tcpIncomingMessage, MAX_MESSAGE_SIZE);
            tcpIncomingMessageBytes = recv(clientSocketTCP, tcpIncomingMessage, MAX_MESSAGE_SIZE, 0);

            while (tcpIncomingMessageBytes > 0) {
                // Stores the client's menu selection
                int menuSelection = tcpIncomingMessage[0] - '0';

                // Removes the menu selection char from the message
                memmove(tcpIncomingMessage, tcpIncomingMessage + 1, tcpIncomingMessageBytes);
                tcpIncomingMessageBytes--;

                if (menuSelection == DEVOWEL_VALUE) {
                    memset(udpMessage, ' ', tcpIncomingMessageBytes);
                    memset(tcpOutgoingMessage, ' ', tcpIncomingMessageBytes);
                    for (int counter = 0; counter < tcpIncomingMessageBytes; counter++) {
                        switch (tcpIncomingMessage[counter]) {
                            case 'A':
                            case 'E':
                            case 'I':
                            case 'O':
                            case 'U':
                            case 'a':
                            case 'e':
                            case 'i':
                            case 'o':
                            case 'u':
                                udpMessage[counter] = tcpIncomingMessage[counter];
                                break;
                            default:
                                tcpOutgoingMessage[counter] = tcpIncomingMessage[counter];
                                break;
                        }
                    }

                    // Sends the server's reply to the client
                    tcpOutgoingMessageBytes = strlen(tcpOutgoingMessage);
                    if (send(clientSocketTCP, tcpOutgoingMessage, tcpOutgoingMessageBytes, 0) < 0) {
                        fprintf(stderr, "Server: TCP send() failed!\n");
                        exit(1);
                    }

                    // Sends the server's reply to the client
                    udpMessageBytes = strlen(udpMessage);
                    if (sendto(serverSocketUDP, udpMessage, udpMessageBytes, MSG_CONFIRM,
                               (const struct sockaddr *) &udpClientAddress, sizeof udpClientAddress) < 0) {
                        fprintf(stderr, "Server: UDP send() failed!\n");
                        exit(1);
                    }
                } else if (menuSelection == ENVOWEL_VALUE) {
                    int len;

                    udpMessageBytes = recvfrom(serverSocketUDP, udpMessage, MAX_MESSAGE_SIZE,
                                               MSG_WAITALL, NULL, (socklen_t *) sizeof clientAddress);
                    udpMessage[udpMessageBytes] = '\0';

                    if (tcpIncomingMessageBytes == udpMessageBytes) {
                        for (int counter = 0; counter < tcpIncomingMessageBytes; counter++) {
                            if (udpMessage[counter] == ' ' && tcpIncomingMessage[counter] != ' ')
                                tcpOutgoingMessage[counter] = tcpIncomingMessage[counter];
                            else if (udpMessage[counter] != ' ' && tcpIncomingMessage[counter] == ' ')
                                tcpOutgoingMessage[counter] = udpMessage[counter];
                        }
                    } else {
                        sprintf(tcpOutgoingMessage, "MALFORMED MESSAGE(S) PROVIDED!");
                    }

                    // Sends the server's reply to the client
                    tcpOutgoingMessageBytes = strlen(tcpOutgoingMessage);
                    if (send(clientSocketTCP, tcpOutgoingMessage, tcpOutgoingMessageBytes, 0) < 0) {
                        fprintf(stderr, "Server: TCP send() failed!\n");
                        exit(1);
                    }
                }

                bzero(tcpIncomingMessage, MAX_MESSAGE_SIZE);
                bzero(tcpOutgoingMessage, MAX_MESSAGE_SIZE);
                bzero(udpMessage, MAX_MESSAGE_SIZE);

                tcpIncomingMessageBytes = recv(clientSocketTCP, tcpIncomingMessage, MAX_MESSAGE_SIZE, 0);
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
