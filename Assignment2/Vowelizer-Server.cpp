#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>


#define PORT     48259
#define MAXLINE 1024
//#define MSG_CONFIRM 0

// Driver code
int main() {
    int sockfd;
    char buffer[MAXLINE];
    char *hello = "Hello from server";
    struct sockaddr_in servaddr, cliaddr;

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Filling server information
    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *) &servaddr,
             sizeof(servaddr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    int len, n;

    len = sizeof(cliaddr);  //len is value/resuslt

    n = recvfrom(sockfd, (char *) buffer, MAXLINE,
                 MSG_WAITALL, (struct sockaddr *) &cliaddr,
                 (socklen_t *) &len);
    buffer[n] = '\0';
    printf("Client : %s\n", buffer);
    sendto(sockfd, (const char *) hello, strlen(hello),
           MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
           len);
    printf("Hello message sent.\n");

    printf("\nTEST: \n");
    char vowels[n];
    char rest[n];
    memset(vowels, ' ', n);
    memset(rest, ' ', n);
    for (int counter = 0; counter < n; counter++) {
        char *characterStatus;
        switch (buffer[counter]) {
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
                characterStatus = "Vowel";
                vowels[counter] = buffer[counter];
                break;
            default:
                characterStatus = "Other";
                rest[counter] = buffer[counter];
                break;
        }
        printf("%c (%d): %s\n", buffer[counter], buffer[counter], characterStatus);
    }
    printf("Vowels: %s\n", vowels);
    printf("Rest: %s\n", rest);

    char merged[n];
    memset(merged, ' ', n);
    for (int counter = 0; counter < n; counter++) {
        if (vowels[counter] == ' ' && rest[counter] != ' ')
            merged[counter] = rest[counter];
        else if (vowels[counter] != ' ' && rest[counter] == ' ')
            merged[counter] = vowels[counter];
    }
    printf("Merged: %s\n", merged);

    return 0;
}
