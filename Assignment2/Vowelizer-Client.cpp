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
#include <cstdlib>

// Global static variables
#define IP "127.0.0.1"
#define PORT 48259
#define MAX_MESSAGE_SIZE 2048
#define EXIT_VALUE 0
#define DEVOWEL_VALUE 1
#define ENVOWEL_VALUE 2

// Function that will print out the menu of options that are supported
void printMenu() {
    printf("\n");
    printf("Please choose an option from the following menu:\n");
    printf("  0 - Exit program\n");
    printf("  1 - Devowel a message\n");
    printf("  2 - Envowel a message\n");
    printf("Your desired menu selection? ");
}

// Main function
int main() {
    // Creates a struct that will store the complete address of the server
    struct sockaddr_in serverAddress;

    // Creates a struct that will store the complete address of the client
    struct sockaddr_in clientAddress;

    // Creates char arrays that will store the message between the client and the server
    char tcpCompleteMessage[MAX_MESSAGE_SIZE];
    char tcpSplitMessage[MAX_MESSAGE_SIZE];
    char udpSplitMessage[MAX_MESSAGE_SIZE];

    // Initialize int variables to store the number of bytes each char array holds
    int tcpCompleteMessageBytes = 0;
    int tcpSplitMessageBytes = 0;
    int udpSplitMessageBytes = 0;

    // Allocates memory to the location that will store the address of the socket
    memset(&serverAddress, 0, sizeof(serverAddress));
    memset(&clientAddress, 0, sizeof(clientAddress));

    // Sets the socket address family to IPv4
    serverAddress.sin_family = AF_INET;

    // Sets the port number that the socket will use
    serverAddress.sin_port = htons(PORT);

    // Sets the complete IP address that the socket will use
    inet_pton(AF_INET, IP, &serverAddress.sin_addr);

    // Creates a TCP socket that will be used to communicate with the server otherwise prints an error and returns if unsuccessful
    int customSocketTCP = socket(AF_INET, SOCK_STREAM, 0);
    if (customSocketTCP == -1) {
        printf("TCP Socket Creation Failed!\n");
        exit(-1);
    }

    // Creates a UDP socket that will be used to communicate with the server otherwise prints an error and returns if unsuccessful
    int customSocketUDP = socket(AF_INET, SOCK_DGRAM, 0);
    if (customSocketUDP == -1) {
        printf("UDP Socket Creation Failed!\n");
        exit(-1);
    }

    // Establishes a connection using the TCP socket otherwise prints an error and returns if unsuccessful
    int connectionStatusTCP = connect(customSocketTCP, (struct sockaddr *) &serverAddress, sizeof(struct sockaddr_in));
    if (connectionStatusTCP == -1) {
        printf("TCP Socket Connection Failed!\n");
        exit(-1);
    }

    // Establishes a connection using the UDP socket otherwise prints an error and returns if unsuccessful
    int connectionStatusUDP = connect(customSocketUDP, (struct sockaddr *) &serverAddress, sizeof(struct sockaddr_in));
    if (connectionStatusUDP == -1) {
        printf("UDP Socket Connection Failed!\n");
        exit(-1);
    }

    // Sets the timeout duration for the UDP socket when receiving messages
    struct timeval timeoutDuration = {10, 0};
    setsockopt(customSocketUDP, SOL_SOCKET, SO_RCVTIMEO, (const char *) &timeoutDuration, sizeof timeoutDuration);

    // Stores the client's UDP socket information
    socklen_t clientAddressLength = sizeof(clientAddress);
    getsockname(customSocketUDP, (struct sockaddr *) &clientAddress, &clientAddressLength);

    // Sends the client's UDP port number to the server over TCP (this way the server knows where to send UDP packets)
    char udpPortNumber[MAX_MESSAGE_SIZE];
    sprintf(udpPortNumber, "%d", ntohs(clientAddress.sin_port));
    int udpPortNumberSent = send(customSocketTCP, udpPortNumber, strlen(udpPortNumber), 0);
    if (udpPortNumberSent == -1) {
        printf("Server <-> Client Configuration Communication Failed!\n");
        exit(-1);
    }

    // Runs until the user specifies to exit the client program
    int userMenuSelection = -1;
    char userInput;
    while (userMenuSelection != EXIT_VALUE) {
        // Prompts the user for a menu selection and stores it
        printMenu();
        scanf("%d", &userMenuSelection);

        if (userMenuSelection == DEVOWEL_VALUE) {
            // Gets the newline character that follows the user's menu selection input
            userInput = getchar();

            // Makes the first char in the message that will be sent the user's menu selection
            tcpCompleteMessageBytes = 1;
            tcpCompleteMessage[0] = userMenuSelection + '0';

            // Gets and stores the message that needs to be sent to the server from the user
            printf("Enter your message: ");
            while ((userInput = getchar()) != '\n') {
                tcpCompleteMessage[tcpCompleteMessageBytes] = userInput;
                tcpCompleteMessageBytes++;
            }

            // Null terminates the message
            tcpCompleteMessage[tcpCompleteMessageBytes] = '\0';

            // Sends the message using the socket otherwise prints an error and returns if unsuccessful
            tcpCompleteMessageBytes = send(customSocketTCP, tcpCompleteMessage, strlen(tcpCompleteMessage), 0);
            if (tcpCompleteMessageBytes == -1) {
                printf("TCP Socket Send Failed!\n");
                continue;
            }

            // Receives the reply using the socket otherwise prints an error and returns if unsuccessful
            tcpSplitMessageBytes = recv(customSocketTCP, tcpSplitMessage, MAX_MESSAGE_SIZE, 0);
            if (tcpSplitMessageBytes == -1) {
                printf("TCP Socket Receive Failed!\n");
                continue;
            }

            if (tcpSplitMessageBytes > 0) {
                /* make sure the message is null-terminated in C */
                tcpSplitMessage[tcpSplitMessageBytes] = '\0';
                printf("Server sent %d bytes of non-vowels using TCP: %s\n", tcpSplitMessageBytes, tcpSplitMessage);
            } else {
                /* an error condition if the server ends unexpectedly */
                printf("TCP Socket Received Nothing!\n");
                continue;
            }

            // Receives the reply using the socket otherwise prints an error and returns if unsuccessful
            int len;
            udpSplitMessageBytes = recvfrom(customSocketUDP, udpSplitMessage, MAX_MESSAGE_SIZE, MSG_WAITALL,
                                            (struct sockaddr *) &serverAddress, (socklen_t *) &len);
            if (udpSplitMessageBytes == -1) {
                printf("UDP Socket Receive Failed!\n");
                continue;
            }

            if (udpSplitMessageBytes > 0) {
                /* make sure the message is null-terminated in C */
                udpSplitMessage[udpSplitMessageBytes] = '\0';
                printf("Server sent %d bytes of vowels using UDP    : %s\n", udpSplitMessageBytes, udpSplitMessage);
            } else {
                /* an error condition if the server ends unexpectedly */
                printf("UDP Socket Received Nothing!\n");
                continue;
            }
        } else if (userMenuSelection == ENVOWEL_VALUE) {

        } else if (userMenuSelection != EXIT_VALUE)
            printf("Invalid menu selection. Please try again.\n");
    }

    // Closes the sockets
    close(customSocketTCP);
    close(customSocketUDP);

    // Ends the program with the success exit code
    exit(0);
}
