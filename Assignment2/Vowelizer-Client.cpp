/* This file contains modified code from the following source(s):
 * https://www.binarytides.com/socket-programming-c-linux-tutorial/ (Socket Programming in C on Linux provided by the Professor under Useful Links on the course page)
 * http://pages.cpsc.ucalgary.ca/~carey/CPSC441/ass0/mypal-server.c (Professor's palindrome checking server)
 * http://pages.cpsc.ucalgary.ca/~carey/CPSC441/solutions/mypal-client.c (Professor's palindrome checking client)
 * UDP_server1.cpp from T04/T10
 * UDP_client1.cpp from T04/T10
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
    // Creates structs that will store the complete addresses of the server and client
    struct sockaddr_in serverAddress;
    struct sockaddr_in clientAddress;

    // Creates char arrays that will store the messages between the client and the server
    char tcpIncomingMessage[MAX_MESSAGE_SIZE];
    char tcpOutgoingMessage[MAX_MESSAGE_SIZE];
    char udpMessage[MAX_MESSAGE_SIZE];

    // Initialize int variables to store the number of bytes each char array holds
    int tcpOutgoingMessageBytes = 0;
    int tcpIncomingMessageBytes = 0;
    int udpMessageBytes = 0;

    // Initializes the incoming and outgoing message arrays with zeroed bytes
    bzero(tcpIncomingMessage, MAX_MESSAGE_SIZE);
    bzero(tcpOutgoingMessage, MAX_MESSAGE_SIZE);
    bzero(udpMessage, MAX_MESSAGE_SIZE);

    // Clears the memory for the locations that will store the addresses
    memset(&serverAddress, 0, sizeof(serverAddress));
    memset(&clientAddress, 0, sizeof(clientAddress));

    // Sets the socket address family to IPv4
    serverAddress.sin_family = AF_INET;

    // Sets the port number that the socket will use
    serverAddress.sin_port = htons(PORT);

    // Sets the complete IP address that the socket will use
    inet_pton(AF_INET, IP, &serverAddress.sin_addr);

    // Creates a TCP socket that will be used to communicate with the server otherwise prints an error and returns if unsuccessful
    int clientSocketTCP = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocketTCP == -1) {
        printf("TCP Socket Creation Failed!\n");
        exit(1);
    }

    // Creates a UDP socket that will be used to communicate with the server otherwise prints an error and returns if unsuccessful
    int clientSocketUDP = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocketUDP == -1) {
        printf("UDP Socket Creation Failed!\n");
        exit(1);
    }

    // Establishes a connection using the TCP socket otherwise prints an error and returns if unsuccessful
    if (connect(clientSocketTCP, (struct sockaddr *) &serverAddress, sizeof(struct sockaddr_in)) == -1) {
        printf("TCP Socket Connection Failed!\n");
        exit(1);
    }

    // Establishes a connection using the UDP socket otherwise prints an error and returns if unsuccessful
    if (connect(clientSocketUDP, (struct sockaddr *) &serverAddress, sizeof(struct sockaddr_in)) == -1) {
        printf("UDP Socket Connection Failed!\n");
        exit(1);
    }

    // Sets the timeout duration for the UDP socket when receiving messages
    struct timeval timeoutDuration = {10, 0};
    setsockopt(clientSocketUDP, SOL_SOCKET, SO_RCVTIMEO, (const char *) &timeoutDuration, sizeof timeoutDuration);

    // Stores the client's UDP socket information
    socklen_t clientAddressLength = sizeof(clientAddress);
    getsockname(clientSocketUDP, (struct sockaddr *) &clientAddress, &clientAddressLength);

    // Sends the client's UDP port number to the server over TCP (this way the server knows where to send UDP packets)
    char udpClientPortNumber[MAX_MESSAGE_SIZE];
    sprintf(udpClientPortNumber, "%d", ntohs(clientAddress.sin_port));
    if (send(clientSocketTCP, udpClientPortNumber, strlen(udpClientPortNumber), 0) == -1) {
        printf("Server <-> Client Configuration Communication Failed!\n");
        exit(1);
    }

    // Prints out the welcome message
    printf("Welcome! This is the client for the Vowelizer program...\n");

    // Runs the client until the user specifies to exit
    int userMenuSelection = -1;
    char userInput;
    while (userMenuSelection != EXIT_VALUE) {
        // Clears the incoming and outgoing message arrays with zeroed bytes
        bzero(tcpIncomingMessage, MAX_MESSAGE_SIZE);
        bzero(tcpOutgoingMessage, MAX_MESSAGE_SIZE);
        bzero(udpMessage, MAX_MESSAGE_SIZE);

        // Resets the variables storing the number of bytes each char array holds
        tcpIncomingMessageBytes = 0;
        tcpOutgoingMessageBytes = 0;
        udpMessageBytes = 0;

        // Prompts the user for a menu selection and stores it
        printMenu();
        scanf("%d", &userMenuSelection);

        // Checks to see if the user specified the devowel message option
        if (userMenuSelection == DEVOWEL_VALUE) {
            // Sets the first char in the message that will be sent the user's menu selection
            tcpOutgoingMessageBytes = 1;
            tcpOutgoingMessage[0] = userMenuSelection + '0';

            // Gets the newline character that follows the input
            userInput = getchar();

            // Gets and stores the message that needs to be sent to the server from the user
            printf("Enter your message to devowel: ");
            while ((userInput = getchar()) != '\n') {
                tcpOutgoingMessage[tcpOutgoingMessageBytes] = userInput;
                tcpOutgoingMessageBytes++;
            }

            // Null terminates the message
            tcpOutgoingMessage[tcpOutgoingMessageBytes] = '\0';

            // Sends the message using the TCP socket otherwise prints an error and returns if unsuccessful
            tcpOutgoingMessageBytes = send(clientSocketTCP, tcpOutgoingMessage, tcpOutgoingMessageBytes, 0);
            if (tcpOutgoingMessageBytes == -1) {
                printf("TCP Socket Send Failed!\n");
                continue;
            }

            // Receives the reply using the TCP socket otherwise prints an error and returns if unsuccessful
            tcpIncomingMessageBytes = recv(clientSocketTCP, tcpIncomingMessage, MAX_MESSAGE_SIZE, 0);
            if (tcpIncomingMessageBytes == -1) {
                printf("TCP Socket Receive Failed!\n");
                continue;
            }

            // Prints out the message if one was received
            if (tcpIncomingMessageBytes > 0) {
                // Null terminates the message
                tcpIncomingMessage[tcpIncomingMessageBytes] = '\0';
                printf("Server sent %d bytes of non-vowels using TCP: %s\n", tcpIncomingMessageBytes,
                       tcpIncomingMessage);
            } else {
                printf("TCP Socket Received Nothing!\n");
                continue;
            }

            // Receives the reply using the socket UDP otherwise prints an error and returns if unsuccessful
            int length;
            udpMessageBytes = recvfrom(clientSocketUDP, udpMessage, MAX_MESSAGE_SIZE, MSG_WAITALL,
                                       (struct sockaddr *) &serverAddress, (socklen_t *) &length);
            if (udpMessageBytes == -1) {
                printf("UDP Socket Receive Failed!\n");
                continue;
            }

            // Prints out the message if one was received
            if (udpMessageBytes > 0) {
                // Null terminates the message
                udpMessage[udpMessageBytes] = '\0';
                printf("Server sent %d bytes of vowels using UDP    : %s\n", udpMessageBytes, udpMessage);
            } else {
                printf("UDP Socket Received Nothing!\n");
                continue;
            }
        }
            // Checks to see if the user specified the envowel message option
        else if (userMenuSelection == ENVOWEL_VALUE) {
            // Sets the first char in the message that will be sent the user's menu selection
            tcpOutgoingMessageBytes = 1;
            tcpOutgoingMessage[0] = userMenuSelection + '0';

            // Gets the newline character that follows the input
            userInput = getchar();

            // Gets and stores the message that needs to be sent to the server from the user
            printf("Enter non-vowel part of the message to envowel: ");
            while ((userInput = getchar()) != '\n') {
                tcpOutgoingMessage[tcpOutgoingMessageBytes] = userInput;
                tcpOutgoingMessageBytes++;
            }

            // Null terminates the message
            tcpOutgoingMessage[tcpOutgoingMessageBytes] = '\0';

            // Required for next text to be print out
            printf("");

            // Gets and stores the message that needs to be sent to the server from the user
            printf("Enter vowel part of the message to envowel    : ");
            while ((userInput = getchar()) != '\n') {
                udpMessage[udpMessageBytes] = userInput;
                udpMessageBytes++;
            }

            // Null terminates the message
            udpMessage[udpMessageBytes] = '\0';

            // Sends the message using the TCP socket otherwise prints an error and returns if unsuccessful
            tcpOutgoingMessageBytes = send(clientSocketTCP, tcpOutgoingMessage, tcpOutgoingMessageBytes, 0);
            if (tcpOutgoingMessageBytes == -1) {
                printf("TCP Socket Send Failed!\n");
                continue;
            }

            // Sends the message using the UDP socket otherwise prints an error and returns if unsuccessful
            if (sendto(clientSocketUDP, udpMessage, udpMessageBytes, MSG_CONFIRM,
                       (const struct sockaddr *) &serverAddress, sizeof serverAddress) < 0) {
                fprintf(stderr, "TCP Socket Send Failed!\n");
                continue;
            }

            // Receives the reply using the TCP socket otherwise prints an error and returns if unsuccessful
            tcpIncomingMessageBytes = recv(clientSocketTCP, tcpIncomingMessage, MAX_MESSAGE_SIZE, 0);
            if (tcpIncomingMessageBytes == -1) {
                printf("TCP Socket Receive Failed!\n");
                continue;
            }

            // Prints out the message if one was received
            if (tcpIncomingMessageBytes > 0) {
                // Null terminates the message
                tcpIncomingMessage[tcpIncomingMessageBytes] = '\0';
                printf("Server sent %d bytes using TCP: %s\n", tcpIncomingMessageBytes, tcpIncomingMessage);
            } else {
                printf("TCP Socket Received Nothing!\n");
                continue;
            }
        } else if (userMenuSelection != EXIT_VALUE)
            printf("Invalid menu selection. Please try again.\n");
    }

    // Closes the sockets
    close(clientSocketTCP);
    close(clientSocketUDP);

    // Ends the program with the success exit code
    exit(0);
}
