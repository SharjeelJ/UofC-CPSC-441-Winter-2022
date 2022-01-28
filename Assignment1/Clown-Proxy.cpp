/* This file contains modified code from the following source(s):
 * https://www.binarytides.com/socket-programming-c-linux-tutorial/ (Socket Programming in C on Linux provided by the Professor under Useful Links on the course page)
 * http://pages.cpsc.ucalgary.ca/~carey/CPSC441/ass0/mypal-server.c (Professor's palindrome checking server)
*/

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <netdb.h>
#include <ctime>

// Global static variables
#define ARRAY_SIZE 2048
#define PROXY_SERVER_PORT 9090

// Creates a socket that will be used by the fork to allow the client to communicate with the proxy
int clientSocket;

// Returns the substring obtained from the first passed in string where the second passed in string is the start boundary and third passed in string is the end boundary (only returns the string between the start and end boundaries)
char *customSubstring(char *originalString, char *resultString, const char *startSubstringTerm,
                      const char *endSubstringTerm) {
    // Finds the pointer position of the second passed in string (start boundary) in the first passed in string (original string)
    char *startSubstringTermPointer = strstr(originalString, startSubstringTerm);

    // Finds the pointer position of the third passed in string (end boundary) in the first passed in string (original string)
    char *endSubstringTermPointer = strstr(startSubstringTermPointer, endSubstringTerm);

    // Calculates the offset necessary to chop off the characters prior to and including the start boundary
    int startOffset = startSubstringTermPointer - originalString + strlen(startSubstringTerm);

    // Calculates the offset necessary to chop off the characters after and including the end boundary
    int endOffset = endSubstringTermPointer - startSubstringTermPointer - strlen(startSubstringTerm);

    // Resets the result char array with zeroed bytes
    memset(resultString, 0, ARRAY_SIZE);

    // Copies the relevant bytes based on the start and end boundaries from the original string and into the result string
    memcpy(resultString, originalString + startOffset, endOffset);

    // Sets the character following the last relevant character to null (null termination will indicate the end of the string)
    resultString[endOffset] = '\0';

    // Returns the result string back to the calling code
    return resultString;
}

// Main function
int main() {
    // Creates a struct that will store the complete address of the socket being created
    struct sockaddr_in proxyAddress = {};

    // Creates char arrays that will store the message between the client and the proxy as well as the message between the proxy and the server that the client would like to talk to
    char *clientMessage = (char *) malloc(0);
    char *serverReply = (char *) malloc(0);

    // Create a temporary buffer array (will be used to store data before it is moved into to the memory allocated arrays)
    char temporaryBuffer[ARRAY_SIZE];

    // Initialize int variables to store the number of bytes each char array holds
    int clientMessageBytes = 0;
    int serverReplyBytes = 0;
    int temporaryBufferBytes = 0;

    // Initializes the incoming and outgoing message and temporary buffer arrays with zeroed bytes
    memset(clientMessage, 0, clientMessageBytes);
    memset(serverReply, 0, serverReplyBytes);
    memset(temporaryBuffer, 0, ARRAY_SIZE);

    // Initialize a char array that will store the result obtained by the customSubstring function
    char customSubstringResult[ARRAY_SIZE];

    // Creates a socket that will be used by the client to initially communicate with the proxy
    int proxySocket;

    // Will be used to store the PID of a child fork
    pid_t pid;

    // Allocates memory to the location that will store the address of the socket
    memset(&proxyAddress, 0, sizeof(proxyAddress));

    // Sets the socket address family to IPv4
    proxyAddress.sin_family = AF_INET;

    // Sets the port number that the socket will use
    proxyAddress.sin_port = htons(PROXY_SERVER_PORT);

    // Sets the complete IP address that the socket will use
    proxyAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    // Creates a socket that will be used to communicate between the proxy and the client otherwise prints an error and returns if unsuccessful
    proxySocket = socket(PF_INET, SOCK_STREAM, 0);
    if (proxySocket == -1) {
        fprintf(stderr, "Proxy Server: socket() failed!\n");
        exit(1);
    }

    // Binds the specific address and port number to the socket otherwise prints an error and returns if unsuccessful
    if (bind(proxySocket, (struct sockaddr *) &proxyAddress, sizeof(struct sockaddr_in)) == -1) {
        fprintf(stderr, "Proxy Server: bind() failed!\n");
        exit(1);
    }

    // Listens on the socket for an incoming client connection
    if (listen(proxySocket, 5) == -1) {
        fprintf(stderr, "Proxy Server: listen() failed!\n");
        exit(1);
    }

    // Prints out the welcome message and TCP port number that the proxy is operating on
    fprintf(stderr, "Clown proxy server running on TCP port %d...\n\n", PROXY_SERVER_PORT);

    // Loops forever to handle client requests
    while (true) {
        // Accepts an incoming socket connection request from the client
        clientSocket = accept(proxySocket, NULL, NULL);
        if (clientSocket == -1) {
            fprintf(stderr, "Proxy Server: accept() failed!\n");
            exit(1);
        }

        // Creates a fork that will deal with the client
        pid = fork();

        // Only proceeds further if the fork creation was successful (0 is returned)
        if (pid < 0) {
            fprintf(stderr, "Proxy Server: fork() failed!\n");
            exit(1);
        } else if (pid == 0) {
            // Closes the inherited socket from the parent as the child does not need it
            close(proxySocket);

            // Receives the client's message using the socket
            memset(temporaryBuffer, 0, ARRAY_SIZE);
            temporaryBufferBytes = recv(clientSocket, temporaryBuffer, ARRAY_SIZE, 0);
            while (temporaryBufferBytes > 0) {
                // Increments the int keeping track of the number of bytes being stored in the memory allocated array
                clientMessageBytes += temporaryBufferBytes;

                // Adjusts the memory allocated array to now be of the newly computed size
                clientMessage = (char *) realloc(clientMessage, clientMessageBytes);

                // Copies the data from the temporary buffer array into the memory allocated array
                memcpy(clientMessage + clientMessageBytes - temporaryBufferBytes, temporaryBuffer,
                       temporaryBufferBytes);

                // Resets the temporary buffer
                memset(temporaryBuffer, 0, temporaryBufferBytes);

                // Breaks the loop if the entire request has been received (checks for the double CRLF at the end of a HTTP request)
                if (strstr(clientMessage, "\r\n\r\n") != NULL)
                    break;

                // Updates the temporary buffer and the int keeping track of it for the next loop iteration (in case there is more to receive than the temporary buffer could hold)
                temporaryBufferBytes = recv(clientSocket, temporaryBuffer, ARRAY_SIZE, 0);
            }

            // Extracts the host information from the header of the client's message
            char serverHost[ARRAY_SIZE];
            strcpy(serverHost, customSubstring(clientMessage, customSubstringResult, "Host: ", "\r\n"));

            // Creates a struct that will store the complete address of the socket being created
            struct sockaddr_in serverAddress = {};

            // Creates a socket that will be used by the proxy to communicate with the server that the client would like to talk to
            int serverHeaderSocket = socket(AF_INET, SOCK_STREAM, 0);
            if (serverHeaderSocket == -1) {
                fprintf(stderr, "Proxy Server: socket() failed!\n");
                exit(1);
            }

            // Creates a socket that will be used by the proxy to communicate with the server that the client would like to talk to
            int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
            if (serverSocket == -1) {
                fprintf(stderr, "Proxy Server: socket() failed!\n");
                exit(1);
            }

            // Creates a pointer that will store the IP address of the server
            char *serverIP;

            // Creates a struct that will allow us to get the IP address of the end server using its host name (using netdb.h)
            struct hostent *hostData;

            // Gets the IP address of the server using its host name
            hostData = gethostbyname(serverHost);

            // Converts the IP address into an ASCII string and stores it
            serverIP = inet_ntoa(*((struct in_addr *) hostData->h_addr_list[0]));

            // Sets the complete IP address that the socket will use
            serverAddress.sin_addr.s_addr = inet_addr(serverIP);

            // Sets the socket address family to IPv4
            serverAddress.sin_family = AF_INET;

            // Sets the port number that the socket will use
            serverAddress.sin_port = htons(80);

            // Connects to the server
            if (connect(serverHeaderSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
                fprintf(stderr, "Proxy Server: connect() failed!\n");
                exit(1);
            }

            // Store the full url that the client is requesting
            char fullURL[ARRAY_SIZE];
            strcpy(fullURL, customSubstring(clientMessage, customSubstringResult, "GET ", " HTTP/1."));

            // Creates a HEAD request to request the header of the url that the client is requesting
            char *headerRequest;
            asprintf(&headerRequest, "HEAD %s HTTP/1.1\r\nHost: pages.cpsc.ucalgary.ca\r\n\r\n", fullURL);

            // Sends the head request using the socket to the server otherwise prints an error and returns if unsuccessful
            if (send(serverHeaderSocket, headerRequest, ARRAY_SIZE, 0) < 0) {
                fprintf(stderr, "Proxy Server: send() failed!\n");
                exit(1);
            }

            // Receives the header using the socket to the server otherwise prints an error and returns if unsuccessful
            char header[ARRAY_SIZE];
            if (recv(serverHeaderSocket, header, ARRAY_SIZE, 0) < 0) {
                fprintf(stderr, "Proxy Server: send() failed!\n");
                exit(1);
            }

            // Closes the header socket with the server
            close(serverHeaderSocket);

            // Stores the Content-Type data from the header
            char contentType[ARRAY_SIZE];
            strcpy(contentType, customSubstring(header, customSubstringResult, "Content-Type: ", "\r\n"));

            // If the Content-Type indicates that the requested page is html then performs the text replacement
            if (strstr(contentType, "text/html") != NULL) {
                // Connects to the server
                if (connect(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
                    fprintf(stderr, "Proxy Server: connect() failed!\n");
                    exit(1);
                }

                // Sends the client's message using the socket to the server otherwise prints an error and returns if unsuccessful
                if (send(serverSocket, clientMessage, clientMessageBytes, 0) < 0) {
                    fprintf(stderr, "Proxy Server: send() failed!\n");
                    exit(1);
                }

                // Receives the server's reply using the socket
                memset(temporaryBuffer, 0, temporaryBufferBytes);
                temporaryBufferBytes = recv(serverSocket, temporaryBuffer, ARRAY_SIZE, 0);
                while (temporaryBufferBytes > 0) {
                    // Increments the int keeping track of the number of bytes being stored in the memory allocated array
                    serverReplyBytes += temporaryBufferBytes;

                    // Adjusts the memory allocated array to now be of the newly computed size
                    serverReply = (char *) realloc(serverReply, serverReplyBytes);

                    // Copies the data from the temporary buffer array into the memory allocated array
                    memcpy(serverReply + serverReplyBytes - temporaryBufferBytes, temporaryBuffer,
                           temporaryBufferBytes);

                    // Resets the temporary buffer
                    memset(temporaryBuffer, 0, temporaryBufferBytes);

                    // Updates the temporary buffer and the int keeping track of it for the next loop iteration (in case there is more to receive than the temporary buffer could hold)
                    temporaryBufferBytes = recv(serverSocket, temporaryBuffer, ARRAY_SIZE, 0);
                }

                // Stores the pointer address of the first instance of "Happy" (otherwise null) in the server's reply
                char *happyPointer = strstr(serverReply, "Happy");

                // Stores the pointer address of a string containing "Silly"
                char *sillyPointer = "Silly";

                // Replaces all instances of "Happy" with "Silly" in the html
                while (happyPointer != NULL) {
                    // Replaces the memory at the pointer location containing "Happy" with the memory from the pointer position containing "Silly"
                    memcpy(happyPointer, sillyPointer, 5);

                    // Updates the pointer address to contain the next instance of "Happy" (otherwise null)
                    happyPointer = strstr(serverReply, "Happy");
                }
            }
                // If the Content-Type indicates that the requested page is a jpeg then returns a redirect
            else if (strstr(contentType, "image/jpeg") != NULL) {
                // Generates a random number between 0 and 1 and uses that as the input for a switch statement that assigns a clown image via a redirect for the current jpeg
                char *redirectURL;
                srand(time(0));
                switch (rand() % 2) {
                    case 0:
                        asprintf(&redirectURL, "HTTP/1.1 302 Found\r\nLocation: %s\r\n\r\n",
                                 "http://pages.cpsc.ucalgary.ca/~carey/CPSC441/ass1/clown1.png");
                        break;
                    case 1:
                        asprintf(&redirectURL, "HTTP/1.1 302 Found\r\nLocation: %s\r\n\r\n",
                                 "http://pages.cpsc.ucalgary.ca/~carey/CPSC441/ass1/clown2.png");
                        break;
                }

                // Stores fake reply from the server indicating that a URL redirect needs to occur for the requested jpeg
                serverReplyBytes = strlen(redirectURL);
                serverReply = (char *) realloc(serverReply, serverReplyBytes);
                memcpy(serverReply, redirectURL, serverReplyBytes);
            } else {
                // Connects to the server
                if (connect(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
                    fprintf(stderr, "Proxy Server: connect() failed!\n");
                    exit(1);
                }

                // Sends the client's message using the socket to the server otherwise prints an error and returns if unsuccessful
                if (send(serverSocket, clientMessage, clientMessageBytes, 0) < 0) {
                    fprintf(stderr, "Proxy Server: send() failed!\n");
                    exit(1);
                }

                // Receives the server's reply using the socket
                memset(temporaryBuffer, 0, temporaryBufferBytes);
                temporaryBufferBytes = recv(serverSocket, temporaryBuffer, ARRAY_SIZE, 0);
                while (temporaryBufferBytes > 0) {
                    // Increments the int keeping track of the number of bytes being stored in the memory allocated array
                    serverReplyBytes += temporaryBufferBytes;

                    // Adjusts the memory allocated array to now be of the newly computed size
                    serverReply = (char *) realloc(serverReply, serverReplyBytes);

                    // Copies the data from the temporary buffer array into the memory allocated array
                    memcpy(serverReply + serverReplyBytes - temporaryBufferBytes, temporaryBuffer,
                           temporaryBufferBytes);

                    // Resets the temporary buffer
                    memset(temporaryBuffer, 0, temporaryBufferBytes);

                    // Updates the temporary buffer and the int keeping track of it for the next loop iteration (in case there is more to receive than the temporary buffer could hold)
                    temporaryBufferBytes = recv(serverSocket, temporaryBuffer, ARRAY_SIZE, 0);
                }
            }

            // Sends the server's reply to the client
            if (send(clientSocket, serverReply, serverReplyBytes, 0) < 0) {
                fprintf(stderr, "Proxy Server: send() failed!\n");
                exit(1);
            }

            // Frees up the memory allocated by the char arrays
            free(clientMessage);
            free(serverReply);

            // Closes the socket with the server
            close(serverSocket);

            // Closes the socket with the client and kills the fork
            fprintf(stderr, "Child process received nothing, so it is exiting now\n");
            close(clientSocket);
            exit(0);
        } else {
            // Informs the user of the child's PID (that will be handling the client and proxy connection) and that the proxy server will now continue to listen for new requests
            fprintf(stderr, "Proxy Server created child process %d to handle the client\n", pid);
            fprintf(stderr, "Proxy Server process going back to listening for new clients now...\n\n");

            // Parent does not need the socket that communicates with the client
            close(clientSocket);
        }
    }
}
