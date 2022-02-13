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

// Use the following values for selecting the mode: 0 = Simple split/merge, 1 = Advanced split/merge, 2 = Custom split/merge
#define MODE 0

// Creates sockets that will be used by the fork to allow the client to communicate with the server
int clientSocketTCP;
int clientSocketUDP;

// Main function
int main() {
    // Creates a struct that will store the complete address of the server
    struct sockaddr_in serverAddress;
    struct sockaddr *clientAddress;

    // Creates char arrays that will store the messages between the client and the server
    char tcpIncomingMessage[MAX_MESSAGE_SIZE];
    char tcpOutgoingMessage[MAX_MESSAGE_SIZE];
    char udpMessage[MAX_MESSAGE_SIZE];

    // Initialize int variables to store the number of bytes each char array holds
    int tcpIncomingMessageBytes = 0;
    int tcpOutgoingMessageBytes = 0;
    int udpMessageBytes = 0;

    // Initializes the incoming and outgoing message arrays with zeroed bytes
    memset(tcpIncomingMessage, 0, MAX_MESSAGE_SIZE);
    memset(tcpOutgoingMessage, 0, MAX_MESSAGE_SIZE);
    memset(udpMessage, 0, MAX_MESSAGE_SIZE);

    // Creates a socket that will be used by the client to initially communicate with the server
    int serverSocketTCP;

    // Will be used to store the PID of a child fork
    pid_t pid;

    // Clears the memory for the locations that will store the addresses
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
        printf("TCP Socket Creation Failed!\n");
        exit(1);
    }

    // Creates a socket that will be used to communicate between the server and the client otherwise prints an error and returns if unsuccessful
    clientSocketUDP = socket(PF_INET, SOCK_DGRAM, 0);
    if (clientSocketUDP == -1) {
        printf("UDP Socket Creation Failed!\n");
        exit(1);
    }

    // Sets the timeout duration for the UDP socket when receiving messages
    struct timeval timeoutDuration = {10, 0};
    setsockopt(clientSocketUDP, SOL_SOCKET, SO_RCVTIMEO, (const char *) &timeoutDuration, sizeof timeoutDuration);

    // Binds the specific address and port number to the TCP socket otherwise prints an error and returns if unsuccessful
    if (bind(serverSocketTCP, (struct sockaddr *) &serverAddress, sizeof(struct sockaddr_in)) == -1) {
        printf("TCP Socket Bind Failed!\n");
        exit(1);
    }

    // Binds the specific address and port number to the UDP socket otherwise prints an error and returns if unsuccessful
    if (bind(clientSocketUDP, (struct sockaddr *) &serverAddress, sizeof(struct sockaddr_in)) == -1) {
        printf("UDP Socket Bind Failed!\n");
        exit(1);
    }

    // Listens on the TCP socket for an incoming client connection
    if (listen(serverSocketTCP, 5) == -1) {
        printf("TCP Socket Listen Failed!\n");
        exit(1);
    }

    // Prints out the welcome message and TCP & UDP port number that the server is operating on
    printf("Vowelizer server running on TCP & UDP port %d...\n\n", PORT);

    // Loops forever to handle client requests
    while (true) {
        // Accepts an incoming TCP socket connection request from the client
        clientSocketTCP = accept(serverSocketTCP, clientAddress, NULL);
        if (clientSocketTCP == -1) {
            printf("TCP Accept Failed!\n");
            exit(1);
        }

        // Creates a fork that will deal with the client
        pid = fork();

        // Only proceeds further if the fork creation was successful (0 is returned)
        if (pid < 0) {
            printf("Fork Creation Failed!\n");
            exit(1);
        } else if (pid == 0) {
            // Closes the inherited TCP socket from the parent as the child does not need it
            close(serverSocketTCP);

            // Gets the information sent by the client and stores the client's UDP port number
            char clientInfo[MAX_MESSAGE_SIZE];
            if (recv(clientSocketTCP, clientInfo, MAX_MESSAGE_SIZE, 0) < 0) {
                printf("Server <-> Client Configuration Communication Failed!\n");
                exit(1);
            }
            int udpPortClient = atoi(clientInfo);

            // Stores the complete client UDP address with the provided port
            sockaddr_in udpClientAddress = reinterpret_cast<const sockaddr_in &>(clientAddress);
            udpClientAddress.sin_port = htons(udpPortClient);

            // Receives the client's message using the TCP socket
            bzero(tcpIncomingMessage, MAX_MESSAGE_SIZE);
            tcpIncomingMessageBytes = recv(clientSocketTCP, tcpIncomingMessage, MAX_MESSAGE_SIZE, 0);

            // Keeps running until the client disconnects from the TCP socket handling their connection
            while (tcpIncomingMessageBytes > 0) {
                // Stores the client's menu selection
                int menuSelection = tcpIncomingMessage[0] - '0';

                // Removes the menu selection char from the message
                memmove(tcpIncomingMessage, tcpIncomingMessage + 1, tcpIncomingMessageBytes);
                tcpIncomingMessageBytes--;

                // Checks to see if the client specified the devowel option
                if (menuSelection == DEVOWEL_VALUE) {
                    // Loops through the entire message and devowels it into two arrays
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
                                tcpOutgoingMessage[counter] = ' ';
                                udpMessage[counter] = tcpIncomingMessage[counter];
                                break;
                            default:
                                tcpOutgoingMessage[counter] = tcpIncomingMessage[counter];
                                udpMessage[counter] = ' ';
                                break;
                        }
                    }

                    // Sends the non-vowel part over TCP to the client
                    tcpOutgoingMessageBytes = strlen(tcpOutgoingMessage);
                    if (send(clientSocketTCP, tcpOutgoingMessage, tcpOutgoingMessageBytes, 0) < 0) {
                        printf("TCP Send Failed!\n");
                        exit(1);
                    }

                    // Sends the vowel part over UDP to the client
                    udpMessageBytes = strlen(udpMessage);
                    if (sendto(clientSocketUDP, udpMessage, udpMessageBytes, MSG_CONFIRM,
                               (const struct sockaddr *) &udpClientAddress, sizeof udpClientAddress) < 0) {
                        printf("UDP Send Failed!\n");
                        exit(1);
                    }
                }
                    // Checks to see if the client specified the envowel option
                else if (menuSelection == ENVOWEL_VALUE) {
                    // Receives the vowel part over UDP from the client
                    udpMessageBytes = recvfrom(clientSocketUDP, udpMessage, MAX_MESSAGE_SIZE,
                                               MSG_WAITALL, NULL, (socklen_t *) sizeof clientAddress);

                    // Null terminates the message
                    udpMessage[udpMessageBytes] = '\0';

                    // Loops through the both parts of the message and envowels it into one array
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

                    // Sends the complete message to the client over TCP
                    tcpOutgoingMessageBytes = strlen(tcpOutgoingMessage);
                    if (send(clientSocketTCP, tcpOutgoingMessage, tcpOutgoingMessageBytes, 0) < 0) {
                        printf("TCP Send Failed!\n");
                        exit(1);
                    }
                }

                // Clears the incoming and outgoing message arrays with zeroed bytes
                bzero(tcpIncomingMessage, MAX_MESSAGE_SIZE);
                bzero(tcpOutgoingMessage, MAX_MESSAGE_SIZE);
                bzero(udpMessage, MAX_MESSAGE_SIZE);

                // Receives the client's message using the TCP socket
                tcpIncomingMessageBytes = recv(clientSocketTCP, tcpIncomingMessage, MAX_MESSAGE_SIZE, 0);
            }

            // Closes the socket with the client and kills the fork
            printf("Child process received nothing, so it is exiting now\n");
            close(clientSocketTCP);
            exit(0);
        } else {
            // Informs the user of the child's PID (that will be handling the client and server connection) and that the server will now continue to listen for new requests
            printf("Server created child process %d to handle the client\n", pid);
            printf("Server process going back to listening for new clients now...\n\n");

            // Parent does not need the socket that communicates with the client
            close(clientSocketTCP);
        }
    }
}
