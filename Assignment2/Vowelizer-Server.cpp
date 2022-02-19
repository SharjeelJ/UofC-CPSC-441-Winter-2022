/* This file contains modified code from the following source(s):
 * https://www.binarytides.com/socket-programming-c-linux-tutorial/ (Socket Programming in C on Linux provided by the Professor under Useful Links on the course page)
 * http://pages.cpsc.ucalgary.ca/~carey/CPSC441/ass0/mypal-server.c (Professor's palindrome checking server)
 * http://pages.cpsc.ucalgary.ca/~carey/CPSC441/solutions/mypal-client.c (Professor's palindrome checking client)
 * https://web.stanford.edu/class/archive/cs/cs106b/cs106b.1126/handouts/220%20Huffman%20Encoding.pdf (Inspiration for the custom split/merge)
 * UDP_server1.cpp from T04/T10
 * UDP_client1.cpp from T04/T10
*/

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>

// Program configurations
#define PORT 48259
// Use the following values for selecting the mode: 0 = Simple split/merge, 1 = Advanced split/merge, 2 = Custom split/merge
#define MODE 0

// Global static variables
#define MAX_MESSAGE_SIZE 2048
#define DEVOWEL_VALUE 1
#define ENVOWEL_VALUE 2

// Creates sockets that will be used by the fork to allow the client to communicate with the server
int clientSocketTCP;
int clientSocketUDP;

// Performs the simple split operation on the passed in arrays in place
void simpleSplit(char *completeMessage, char *noVowelMessage, char *vowelMessage) {
    // Sets both the arrays to contain a blank space string if the string to be devoweled is empty
    if (strlen(completeMessage) == 0) {
        sprintf(noVowelMessage, " ");
        sprintf(vowelMessage, " ");
    } else
        // Loops through the entire message and devowels it into two arrays
        for (int counter = 0; counter < strlen(completeMessage); counter++) {
            switch (completeMessage[counter]) {
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
                    noVowelMessage[counter] = ' ';
                    vowelMessage[counter] = completeMessage[counter];
                    break;
                default:
                    noVowelMessage[counter] = completeMessage[counter];
                    vowelMessage[counter] = ' ';
                    break;
            }
        }
}

// Performs the simple merge operation on the passed in arrays in place
void simpleMerge(char *completeMessage, char *noVowelMessage, char *vowelMessage) {
    // Sets the message to be an empty string if both the parts of the message are empty
    if (strlen(noVowelMessage) == 0 && strlen(vowelMessage) == 0) sprintf(completeMessage, " ");
        // Sets the message to be the vowel part if the non-vowel part is empty
    else if (strlen(noVowelMessage) == 0 && strlen(vowelMessage) != 0) strcpy(completeMessage, vowelMessage);
        // Sets the message to be the non-vowel part if the vowel part is empty
    else if (strlen(noVowelMessage) != 0 && strlen(vowelMessage) == 0) strcpy(completeMessage, noVowelMessage);
        // If both parts of the message are of equal length then loops through them and envowels them into one array
    else if (strlen(noVowelMessage) == strlen(vowelMessage)) {
        for (int counter = 0; counter < strlen(noVowelMessage); counter++) {
            // Checks to see if the non-vowel part is contributing to the message as the vowel part has a space
            if (vowelMessage[counter] == ' ' && noVowelMessage[counter] != ' ')
                completeMessage[counter] = noVowelMessage[counter];
                // Checks to see if the vowel part is contributing to the message as the non-vowel part has a space
            else if (vowelMessage[counter] != ' ' && noVowelMessage[counter] == ' ')
                completeMessage[counter] = vowelMessage[counter];
                // If both parts had a space
            else completeMessage[counter] = ' ';
        }
    }
        // Sets an error message as the length of both parts of the message are not equal
    else sprintf(completeMessage, "Malformed Message(s) Provided!");
}

// Performs the advanced split operation on the passed in arrays in place
void advancedSplit(char *completeMessage, char *noVowelMessage, char *vowelMessage) {
    // Sets both the arrays to contain a blank space string if the string to be devoweled is empty
    if (strlen(completeMessage) == 0) {
        sprintf(noVowelMessage, " ");
        sprintf(vowelMessage, " ");
    } else {
        // Loops through the entire message and devowels it into two arrays
        int displacementValue = 0;
        for (int counter = 0; counter < strlen(completeMessage); counter++) {
            switch (completeMessage[counter]) {
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
                    // Adds the vowel's displacement and the vowel to the vowel message and resets the vowel displacement counter
                    char temp[2];
                    sprintf(temp, "%d", displacementValue);
                    strncat(vowelMessage, &temp[0], 1);
                    strncat(vowelMessage, &completeMessage[counter], 1);
                    displacementValue = 0;
                    break;
                default:
                    // Adds the non-vowel to the non-vowel message and increments the vowel displacement counter
                    strncat(noVowelMessage, &completeMessage[counter], 1);
                    displacementValue++;
                    break;
            }
        }
        // Sets an empty string for the no vowel message if there are no non-vowels
        if (strlen(noVowelMessage) == 0) sprintf(noVowelMessage, " ");
            // Sets an empty string for the vowel message if there are no vowels
        else if (strlen(vowelMessage) == 0) sprintf(vowelMessage, " ");
    }
}

// Performs the advanced merge operation on the passed in arrays in place
void advancedMerge(char *completeMessage, char *noVowelMessage, char *vowelMessage) {
    // Loops through both parts of the message and envowels it into one array
    if (strlen(noVowelMessage) == 0 && strlen(vowelMessage) == 0)
        sprintf(completeMessage, " ");
    else {
        int completeMessageLength = 0;
        int displacementValue = 0;
        for (int counter = 0; counter <= strlen(vowelMessage); counter += 2) {
            if (atoi(&vowelMessage[counter]) == displacementValue) {
                strncat(completeMessage, &vowelMessage[counter + 1], 1);
                displacementValue = 0;
            } else {
                if (completeMessageLength < strlen(noVowelMessage)) {
                    strncat(completeMessage, &noVowelMessage[completeMessageLength], 1);
                    displacementValue++;
                    completeMessageLength++;
                    counter -= 2;
                }
            }
        }
        while (completeMessageLength < (strlen(noVowelMessage) + (strlen(vowelMessage) / 2))) {
            strncat(completeMessage, &noVowelMessage[completeMessageLength], 1);
            completeMessageLength++;
        }
    }
}

// Helper function for the custom split/merge function that converts a vowel's displacement value and the vowel into a single character (does not preserve case)
int encodeVowel(char vowel, int displacement) {
    if (vowel == 'A' || vowel == 'a') return 33 + displacement - '0';
    else if (vowel == 'E' || vowel == 'e') return 43 + displacement - '0';
    else if (vowel == 'I' || vowel == 'i') return 53 + displacement - '0';
    else if (vowel == 'O' || vowel == 'o') return 63 + displacement - '0';
    else if (vowel == 'U' || vowel == 'u') return 73 + displacement - '0';
}

// Helper function for the custom split/merge function that decodes a single character into a vowel's displacement value and the vowel (does not preserve case)
void decodeVowel(char encodedVowel, char decodedVowelAndDisplacement[]) {
    if (encodedVowel >= 33 && encodedVowel < 43) {
        decodedVowelAndDisplacement[0] = encodedVowel - 33 + '0';
        decodedVowelAndDisplacement[1] = 'a';
    } else if (encodedVowel >= 43 && encodedVowel < 53) {
        decodedVowelAndDisplacement[0] = encodedVowel - 43 + '0';
        decodedVowelAndDisplacement[1] = 'e';

    } else if (encodedVowel >= 53 && encodedVowel < 63) {
        decodedVowelAndDisplacement[0] = encodedVowel - 53 + '0';
        decodedVowelAndDisplacement[1] = 'i';

    } else if (encodedVowel >= 63 && encodedVowel < 73) {
        decodedVowelAndDisplacement[0] = encodedVowel - 63 + '0';
        decodedVowelAndDisplacement[1] = 'o';

    } else if (encodedVowel >= 73 && encodedVowel < 83) {
        decodedVowelAndDisplacement[0] = encodedVowel - 73 + '0';
        decodedVowelAndDisplacement[1] = 'u';
    }
}

// Helper function for the custom split/merge function that encodes the capitalization of the vowels from the vowelMessage of the advanced split function into (# of vowels / 5) number of characters (rounded up)
void encodeCapitalization(char encodedCapitalizations[], char vowelMessage[]) {
    int currentVowelNumber = 0;
    while (currentVowelNumber < strlen(encodedCapitalizations)) {
        // Only uses the bits indicated with X in 01XXXXX0 for encoding capitalizations (left -> right) where one bit represents a single character's capitalization where 1 is capitalized and 0 is non-capitalized
        int bitMask = 64;
        for (int counter = 0; counter < 5; counter++) {
            if (currentVowelNumber < strlen(encodedCapitalizations)) {
                // If the current character being checked is the vowel's displacement value then decrements the counter (keeps track of how many characters have been encoded) and continues to the next character (a vowel)
                if (encodedCapitalizations[currentVowelNumber] >= 48 &&
                    encodedCapitalizations[currentVowelNumber] <= 57) {
                    counter--;
                }
                    // If the vowel is not capitalized then sets a 0 bit
                else if (encodedCapitalizations[currentVowelNumber] >= 97 &&
                         encodedCapitalizations[currentVowelNumber] <= 122)
                    bitMask = bitMask | (1 >> (counter + 1));
                    // If the vowel is capitalized then sets a 1 bit
                else if (encodedCapitalizations[currentVowelNumber] >= 65 &&
                         encodedCapitalizations[currentVowelNumber] <= 90)
                    bitMask = bitMask | (64 >> (counter + 1));
            }
                // Breaks the loop if all the usable bits in a single character have been exhausted (will start using a new character)
            else
                break;

            // Increments the counter keeping track of the number of vowel's encoded
            currentVowelNumber++;
        }
        // Stores the vowel (containing the capitalization information for up to 5 vowels) at the end of the vowel message (after the space delimiter)
        char temp[1];
        temp[0] = bitMask;
        strncat(vowelMessage, temp, 1);
    }
}

// Helper function for the custom split/merge function that decodes and adjusts the capitalization of the vowels obtained by decoding the custom encoding for the vowels (that does not preserve case by itself otherwise)
void decodeCapitalization(char encodedCapitalizations[], char vowelMessage[]) {
    for (int counter = 1; counter < strlen(vowelMessage); counter += 2) {
        if (encodedCapitalizations[(counter / 2) / 5] & (64 >> (((counter / 2) % 5) + 1)))
            if (vowelMessage[counter] == 'a') vowelMessage[counter] = 'A';
            else if (vowelMessage[counter] == 'e') vowelMessage[counter] = 'E';
            else if (vowelMessage[counter] == 'i') vowelMessage[counter] = 'I';
            else if (vowelMessage[counter] == 'o') vowelMessage[counter] = 'O';
            else if (vowelMessage[counter] == 'u') vowelMessage[counter] = 'U';
    }
}

// Performs the custom split operation on the passed in array in place
void customSplit(char *vowelMessage) {
    // Creates arrays that will store the results
    char result[MAX_MESSAGE_SIZE];
    char encodedCapitalizations[MAX_MESSAGE_SIZE];
    bzero(result, MAX_MESSAGE_SIZE);
    bzero(encodedCapitalizations, MAX_MESSAGE_SIZE);

    // Gets the encoded capitalizations for the vowel message
    encodeCapitalization(vowelMessage, encodedCapitalizations);

    // Encodes only the vowels from the vowel message and adds the encoded vowels to the result
    for (int counter = 0; counter < strlen(vowelMessage); counter += 2)
        result[counter / 2] = encodeVowel(vowelMessage[counter + 1], vowelMessage[counter]);

    // Adds a space at the end of the encoded vowels (to sepereate the encoded vowels from the capitalization encoding)
    result[strlen(vowelMessage) / 2] = ' ';

    // Stores the encoded capitalizations at the end of the encoded vowels and stores the result into the vowel message array
    strncat(result, encodedCapitalizations, strlen(encodedCapitalizations));
    strncpy(vowelMessage, result, strlen(result));
    vowelMessage[strlen(result)] = '\0';
}

// Performs the custom merge operation on the passed in array in place
void customMerge(char *vowelMessage) {
    // Creates an array that will store the results
    char result[MAX_MESSAGE_SIZE];
    bzero(result, MAX_MESSAGE_SIZE);

    // Stores pointers to the encoded encodedVowels and capitalizations encoding into separate arrays
    char *encodedVowels = strtok(vowelMessage, " ");
    char *encodedCapitalizations = strtok(NULL, " ");

    // Loops through the vowels and adds the decoded values to the result array
    for (int counter = 0; counter < strlen(encodedVowels); counter++) {
        char temp[2];
        decodeVowel(encodedVowels[counter], temp);
        result[counter * 2] = temp[0];
        result[counter * 2 + 1] = temp[1];
    }

    // Adjusts the result array using the encoded capitalizations to preserve vowel cases
    decodeCapitalization(encodedCapitalizations, result);

    // Stores the result into the vowel message array
    strncpy(vowelMessage, result, strlen(result));
}

// Main function
int main() {
    // Creates structs that will store the complete address of the server and client
    struct sockaddr_in serverAddress{};
    struct sockaddr_in clientAddress{};

    // Creates char arrays that will store the messages between the client and the server
    char tcpIncomingMessage[MAX_MESSAGE_SIZE];
    char tcpOutgoingMessage[MAX_MESSAGE_SIZE];
    char udpMessage[MAX_MESSAGE_SIZE];

    // Initialize int variables to store the number of bytes each char array holds
    int tcpIncomingMessageBytes = 0;
    int tcpOutgoingMessageBytes = 0;
    int udpMessageBytes = 0;

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

    // Creates a socket that will be used to communicate between the server and the client otherwise prints an error and exits if unsuccessful
    serverSocketTCP = socket(PF_INET, SOCK_STREAM, 0);
    if (serverSocketTCP == -1) {
        printf("TCP Socket Creation Failed!\n");
        exit(1);
    }

    // Creates a socket that will be used to communicate between the server and the client otherwise prints an error and exits if unsuccessful
    clientSocketUDP = socket(PF_INET, SOCK_DGRAM, 0);
    if (clientSocketUDP == -1) {
        printf("UDP Socket Creation Failed!\n");
        exit(1);
    }

    // Sets the timeout duration for the UDP socket when receiving messages
    struct timeval timeoutDuration = {10, 0};
    setsockopt(clientSocketUDP, SOL_SOCKET, SO_RCVTIMEO, (const char *) &timeoutDuration, sizeof timeoutDuration);

    // Binds the specific address and port number to the TCP socket otherwise prints an error and exits if unsuccessful
    if (bind(serverSocketTCP, (struct sockaddr *) &serverAddress, sizeof(struct sockaddr_in)) == -1) {
        printf("TCP Socket Bind Failed!\n");
        exit(1);
    }

    // Binds the specific address and port number to the UDP socket otherwise prints an error and exits if unsuccessful
    if (bind(clientSocketUDP, (struct sockaddr *) &serverAddress, sizeof(struct sockaddr_in)) == -1) {
        printf("UDP Socket Bind Failed!\n");
        exit(1);
    }

    // Listens on the TCP socket for an incoming client connection otherwise prints an error and exits if unsuccessful
    if (listen(serverSocketTCP, 5) == -1) {
        printf("TCP Socket Listen Failed!\n");
        exit(1);
    }

    // Prints out the welcome message and TCP & UDP port number that the server is operating on
    printf("Vowelizer server running on TCP & UDP port %d...\n\n", PORT);

    // Loops forever to handle client requests
    while (true) {
        // Accepts an incoming TCP socket connection request from the client otherwise prints an error and exits if unsuccessful
        socklen_t clientAddressLength = sizeof(clientAddress);
        clientSocketTCP = accept(serverSocketTCP, (struct sockaddr *) &clientAddress, &clientAddressLength);
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

            // Gets the information sent by the client and stores the client's UDP port number otherwise prints an error and exits if unsuccessful
            char clientInfo[5];
            if (recv(clientSocketTCP, clientInfo, 5, 0) < 0) {
                printf("Server <-> Client Configuration Communication Failed!\n");
                exit(1);
            }
            int udpClientPortNumber = atoi(clientInfo);

            // Stores the complete client UDP address with the provided port
            sockaddr_in udpClientAddress = clientAddress;
            udpClientAddress.sin_port = htons(udpClientPortNumber);

            // Keeps running until the client disconnects from the TCP socket handling their connection
            while (tcpIncomingMessageBytes >= 0) {
                // Clears the incoming and outgoing message arrays with zeroed bytes
                bzero(tcpIncomingMessage, MAX_MESSAGE_SIZE);
                bzero(tcpOutgoingMessage, MAX_MESSAGE_SIZE);
                bzero(udpMessage, MAX_MESSAGE_SIZE);

                // Resets the variables storing the number of bytes each char array holds
                tcpIncomingMessageBytes = 0;
                tcpOutgoingMessageBytes = 0;
                udpMessageBytes = 0;

                // Receives the client's message using the TCP socket
                tcpIncomingMessageBytes = recv(clientSocketTCP, tcpIncomingMessage, MAX_MESSAGE_SIZE, 0);

                // Stores the client's menu selection
                int menuSelection = tcpIncomingMessage[0] - '0';

                // Removes the menu selection char from the message
                memmove(tcpIncomingMessage, tcpIncomingMessage + 1, tcpIncomingMessageBytes);
                tcpIncomingMessageBytes--;

                // Checks to see if the client specified the devowel option
                if (menuSelection == DEVOWEL_VALUE) {
                    // Calls the appropriate function based on what mode is set
                    switch (MODE) {
                        case 0:
                            simpleSplit(tcpIncomingMessage, tcpOutgoingMessage, udpMessage);
                            break;
                        case 1:
                            advancedSplit(tcpIncomingMessage, tcpOutgoingMessage, udpMessage);
                            break;
                        case 2:
                            advancedSplit(tcpIncomingMessage, tcpOutgoingMessage, udpMessage);
                            if (strlen(udpMessage) > 1) customSplit(udpMessage);
                            break;
                    }

                    // Sends the non-vowel part over TCP to the client otherwise prints an error and exits if unsuccessful
                    tcpOutgoingMessageBytes = strlen(tcpOutgoingMessage);
                    if (send(clientSocketTCP, tcpOutgoingMessage, tcpOutgoingMessageBytes, 0) < 0) {
                        printf("TCP Send Failed!\n");
                        exit(1);
                    }

                    // Sends the vowel part over UDP to the client otherwise prints an error and exits if unsuccessful
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
                    if (udpMessageBytes == -1) {
                        // Sends an error message to the client over TCP otherwise prints an error and exits if unsuccessful
                        sprintf(tcpOutgoingMessage, "UDP Message Missing!");
                        tcpOutgoingMessageBytes = strlen(tcpOutgoingMessage);
                        if (send(clientSocketTCP, tcpOutgoingMessage, tcpOutgoingMessageBytes, 0) < 0) {
                            printf("TCP Send Failed!\n");
                            exit(1);
                        }
                        continue;
                    }

                    // Null terminates the message
                    udpMessage[udpMessageBytes] = '\0';

                    // Calls the appropriate function based on what mode is set
                    switch (MODE) {
                        case 0:
                            simpleMerge(tcpOutgoingMessage, tcpIncomingMessage, udpMessage);
                            break;
                        case 1:
                            advancedMerge(tcpOutgoingMessage, tcpIncomingMessage, udpMessage);
                            break;
                        case 2:
                            if (strlen(udpMessage) > 1) customMerge(udpMessage);
                            advancedMerge(tcpOutgoingMessage, tcpIncomingMessage, udpMessage);
                            break;
                    }

                    // Sends the complete message to the client over TCP otherwise prints an error and exits if unsuccessful
                    tcpOutgoingMessageBytes = strlen(tcpOutgoingMessage);
                    if (send(clientSocketTCP, tcpOutgoingMessage, tcpOutgoingMessageBytes, 0) < 0) {
                        printf("TCP Send Failed!\n");
                        exit(1);
                    }
                }
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
