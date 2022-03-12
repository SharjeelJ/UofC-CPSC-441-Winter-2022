#include <cstdio>
#include <cstdlib>
#include <cstring>

int main() {
    FILE *filePointer;
    char filePath[] = "soccer.txt";
    char fileCurrentLine[100];

    if ((filePointer = fopen(filePath, "r")) == NULL) {
        printf("Cannot open file!");
        exit(1);
    }

    while (fgets(fileCurrentLine, 100, filePointer) != NULL) {
        printf("%s", fileCurrentLine);

        double packetArrivalTime = atof(strtok(fileCurrentLine, " "));
        int packetSize = atoi(strtok(NULL, " "));
        printf("Time: %f\nSize: %d\n\n", packetArrivalTime, packetSize);
    }

    fclose(filePointer);
    return 0;
}
