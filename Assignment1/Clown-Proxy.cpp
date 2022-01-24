#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <netinet/in.h>
#include <csignal>
#include <cstring>
#include <arpa/inet.h>
#include <regex.h>
#include <netdb.h>

#define MAX_MESSAGE_LENGTH 4000
#define PROXY_SERVER_PORT 9090

// Global variables
int clientChildSocket;

// Returns the substring obtained from the first passed in string where the second passed in string is the start boundary and third passed in string is the end boundary (only returns the string between the start and end boundaries)
char *customSubstring(char *originalString, const char *startSubstringTerm, const char *endSubstringTerm) {
    // Finds the pointer position of the second passed in string (start boundary) in the first passed in string (original string)
    char *startSubstringTermPointer = strstr(originalString, startSubstringTerm);

    // Finds the pointer position of the third passed in string (end boundary) in the first passed in string (original string)
    char *endSubstringTermPointer = strstr(startSubstringTermPointer, endSubstringTerm);

    // Calculates the offset necessary to chop off the characters prior to and including the start boundary
    int startOffset = startSubstringTermPointer - originalString + strlen(startSubstringTerm);

    // Calculates the offset necessary to chop off the characters after and including the end boundary
    int endOffset = endSubstringTermPointer - startSubstringTermPointer - strlen(startSubstringTerm) - 1;

    // Creates a char array that will store the result string
    char resultString[MAX_MESSAGE_LENGTH];

    // Copies the relevant bytes based on the start and end boundaries from the original string and into the result string
    memcpy(resultString, originalString + startOffset, endOffset);

    // Sets the character following the last relevant character to null (null termination will indicate the end of the string)
    resultString[endOffset + 1] = '\0';

    // Returns the result string back to the calling code
    return resultString;
}

// Main function
int main() {
    // Creates a struct that will store the complete address of the socket being created
    struct sockaddr_in clientServer = {};

    // Creates char arrays that will store the incoming and outgoing message bytes between the client and the server
    char clientIncomingMessage[MAX_MESSAGE_LENGTH];
    char clientOutgoingMessage[MAX_MESSAGE_LENGTH];

    // Initializes the incoming and outgoing message arrays with zeroed bytes
    bzero(clientIncomingMessage, MAX_MESSAGE_LENGTH);
    bzero(clientOutgoingMessage, MAX_MESSAGE_LENGTH);

    // Creates a socket that will be used by the server to communicate with the client
    int mainServerSocket;

    // Create an int that will store the number of bytes contained within a message between the client and the server
    int bytes;

    // Stores the PID of a child fork
    pid_t pid;

    // Allocates memory to the location that will store the address of the socket
    memset(&clientServer, 0, sizeof(clientServer));

    // Sets the socket address family to IPv4
    clientServer.sin_family = AF_INET;

    // Sets the port number that the socket will use
    clientServer.sin_port = htons(PROXY_SERVER_PORT);

    // Sets the complete IP address that the socket will use
    clientServer.sin_addr.s_addr = htonl(INADDR_ANY);

    // Creates a socket that will be used to communicate with the server by the client otherwise prints an error and returns if unsuccessful
    mainServerSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (mainServerSocket == -1) {
        fprintf(stderr, "Server: socket() call failed!\n");
        exit(1);
    }

    // Binds the specific address and port number to the socket otherwise prints an error and returns if unsuccessful
    if (bind(mainServerSocket, (struct sockaddr *) &clientServer, sizeof(struct sockaddr_in)) == -1) {
        fprintf(stderr, "Server: bind() call failed!\n");
        exit(1);
    }

    // Listens on the socket for incoming client connections
    if (listen(mainServerSocket, 5) == -1) {
        fprintf(stderr, "Server: listen() call failed!\n");
        exit(1);
    }

    fprintf(stderr, "Hello there! I am a server for a clown proxy!!\n");
    fprintf(stderr, "I am running on TCP port %d for you...\n\n", PROXY_SERVER_PORT);

    // Loops forever to handle client requests
    for (;;) {
        // Accepts an incoming socket connection request
        clientChildSocket = accept(mainServerSocket, NULL, NULL);
        if (clientChildSocket == -1) {
            fprintf(stderr, "Server: accept() call failed!\n");
            exit(1);
        }

        // Creates a fork that will deal with the client
        pid = fork();

        // Only proceeds further if the fork creation was successful (0 is returned)
        if (pid < 0) {
            fprintf(stderr, "Server: fork() call failed! OMG!!\n");
            exit(1);
        } else if (pid == 0) {
            // Closes the inherited socket from the parent as the child does not need it
            close(mainServerSocket);

            // Receives the incoming message from the client and stores it
            bytes = recv(clientChildSocket, clientIncomingMessage, MAX_MESSAGE_LENGTH, 0);

            // Loops through the entire message
            while (bytes > 0) {
                printf("Server's child process received %d bytes: '%s'\n", bytes, clientIncomingMessage);

                // Extracts the host information from the header of the client's message
                char host[MAX_MESSAGE_LENGTH];
                strcpy(host, customSubstring(clientIncomingMessage, "Host: ", "\n"));

                // Creates a struct that will store the complete address of the socket being created
                struct sockaddr_in proxyServer = {};

                // Creates a char array that will store the reply the server receives from destination that the client would like to talk to
                char proxyServerReply[MAX_MESSAGE_LENGTH];

                // Creates a socket that will be used by the server to communicate with the end server
                int proxySocket = socket(AF_INET, SOCK_STREAM, 0);
                if (proxySocket == -1) {
                    printf("Could not create socket");
                }

                // Creates a pointer that will store the IP address of the end server
                char *IPbuffer;

                // Creates a struct that will allow us to get the IP address of the end server using its host name (using netdb.h)
                struct hostent *host_entry;

                // Gets the IP address of the end server using its host name
                host_entry = gethostbyname(host);

                // Converts the IP address into an ASCII string and stores it
                IPbuffer = inet_ntoa(*((struct in_addr *) host_entry->h_addr_list[0]));

                // TODO: Remove
                printf("Host IP: %s\n", IPbuffer);

                // Sets the complete IP address that the socket will use
                proxyServer.sin_addr.s_addr = inet_addr(IPbuffer);

                // Sets the socket address family to IPv4
                proxyServer.sin_family = AF_INET;

                // Sets the port number that the socket will use
                proxyServer.sin_port = htons(80);

                // Connects to the end Server
                if (connect(proxySocket, (struct sockaddr *) &proxyServer, sizeof(proxyServer)) < 0) {
                    puts("connect error");
                    return 1;
                }

                // Sends the client's message using the socket to the end server otherwise prints an error and returns if unsuccessful
                if (send(proxySocket, clientIncomingMessage, strlen(clientIncomingMessage), 0) < 0) {
                    puts("Send failed");
                    return 1;
                }

                // TODO: Remove
                puts("Data Send\n");

                // Receives the reply using the socket otherwise prints an error and returns if unsuccessful
                if (recv(proxySocket, proxyServerReply, MAX_MESSAGE_LENGTH, 0) < 0) {
                    puts("recv failed");
                }

                // TODO: Remove
                puts("Reply received\n");
                puts(proxyServerReply);

                // Extracts the content length information from the header of the end server's reply
                char replyLength[MAX_MESSAGE_LENGTH];
                strcpy(replyLength, customSubstring(proxyServerReply, "Content-Length: ", "\n"));

                // TODO: Remove
                printf("\nLENGTH: %d\n", atoi(replyLength));

                // Forwards the end server's reply to the client
                bytes = send(clientChildSocket, proxyServerReply, strlen(proxyServerReply), 0);
                if (bytes < 0) {
                    fprintf(stderr, "Server: send() failed!\n");
                }

                // Closes the socket with the end server
                close(proxySocket);

                // Clears out the message arrays
                bzero(clientIncomingMessage, MAX_MESSAGE_LENGTH);
                bzero(clientOutgoingMessage, MAX_MESSAGE_LENGTH);

                // Checks to see if there is any more incoming messages from the client
                bytes = recv(clientChildSocket, clientIncomingMessage, MAX_MESSAGE_LENGTH, 0);
            }

            fprintf(stderr, "Child process received nothing, so it is exiting now\n");
            close(clientChildSocket);
            exit(0);
        } else {
            fprintf(stderr, "Server created child process %d to handle that client\n", pid);
            fprintf(stderr, "Main clientServer process going back to listening for new clients now...\n\n");

            // Parent does not need the socket that communicates with the client
            close(clientChildSocket);
        }
    }
}
