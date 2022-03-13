/* This file contains modified code from the following source(s):
 * 7-Network-Simulation.pdf from T02/T03
*/

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <queue>
#include <tuple>
#include <algorithm>

// Router buffer size (maximum number of queued packets that can be held)
#define BUFFER_SIZE 100

// Router transmission speed in megabits per second (mbps)
#define TRANSMISSION_SPEED_MEGABITS 3.0

// File path of the trace file being simulated
#define TRACE_FILE "starwars.txt"

// Event types used by the vector tuple
#define PACKET_ARRIVAL 0
#define PACKET_DEPARTURE 1

// Custom comparator that will be used to sort the events vector based on the time of the event in descending order (upcoming event will be at the end of the vector)
bool customComparator(std::tuple<double, int, int> const &event1, std::tuple<double, int, int> const &event2) {
    return std::get<0>(event1) > std::get<0>(event2);
}

// Main function
int main() {
    // Computes and stores the router's transmission speed with different units
    double transmissionSpeedBits = TRANSMISSION_SPEED_MEGABITS * 1000000.0;
    double transmissionSpeedBytes = transmissionSpeedBits / 8.0;

    // Vector to store all the events in the simulation (each event is of the form <Time of Event, Byte Size, Type of Event>)
    std::vector<std::tuple<double, int, int>> events;

    // Initialize variables that will be used for the trace file's IO
    FILE *filePointer;
    char filePath[] = TRACE_FILE;
    char fileCurrentLine[100];

    // Opens the trace file otherwise prints an error and quits
    if ((filePointer = fopen(filePath, "r")) == nullptr) {
        printf("Cannot open trace file!");
        exit(1);
    }

    // Populates the vector with the contents of the entire trace file
    while (fgets(fileCurrentLine, 100, filePointer) != nullptr) {
        double packetArrivalTime = atof(strtok(fileCurrentLine, " "));
        int packetSize = atoi(strtok(nullptr, " "));
        events.emplace_back(std::make_tuple(packetArrivalTime, packetSize, PACKET_ARRIVAL));
    }

    // Closes the trace file's IO
    fclose(filePointer);

    // Sorts the vector containing all the events in the simulation in descending order based on the time of each event (upcoming event is at the end)
    std::sort(events.begin(), events.end(), &customComparator);

    // Initialize statistical variables for the simulation
    int packetsReceived = 0, packetsSent = 0, packetsLost = 0;
    int bytesReceived = 0, bytesSent = 0, bytesLost = 0;
    int currentBufferOccupancy = 0, currentBufferSize = 0;
    int maxBufferOccupancyHit = 0, maxBufferSizeHit = 0;
    double cumulativeDelay = 0;

    // Initialize state variables for the simulation
    double currentTime = 0.0;

    // Loops through all the stored events (performs the simulation)
    while (!events.empty()) {
        double currentEventTime = std::get<0>(events.back());
        int currentEventSize = std::get<1>(events.back());
        int currentEventType = std::get<2>(events.back());

        currentTime = currentEventTime;

        switch (currentEventType) {
            case PACKET_ARRIVAL:
                packetsReceived++;
                bytesReceived += currentEventSize;

                if (currentBufferOccupancy >= BUFFER_SIZE) {
                    packetsLost++;
                    bytesLost += currentEventSize;
                    events.pop_back();
                } else {
                    currentBufferOccupancy++;
                    currentBufferSize += currentEventSize;
                    events.pop_back();
                    events.emplace_back(
                            std::make_tuple(currentTime + currentBufferSize / transmissionSpeedBytes, currentEventSize,
                                            PACKET_DEPARTURE));
                }
                break;
            case PACKET_DEPARTURE:
                packetsSent++;
                bytesSent += currentEventSize;
                currentBufferOccupancy--;
                currentBufferSize -= currentEventSize;
                events.pop_back();
                break;
        }

        // Checks to see if the record number of packets stored in the buffer has been exceeded
        if (currentBufferOccupancy > maxBufferOccupancyHit)
            maxBufferOccupancyHit = currentBufferOccupancy;

        // Checks to see if the record number of bytes stored in the buffer has been exceeded
        if (currentBufferSize > maxBufferSizeHit)
            maxBufferSizeHit = currentBufferSize;

        // Sorts the vector containing all the events in the simulation in descending order based on the time of each event (upcoming event is at the end)
        std::sort(events.begin(), events.end(), &customComparator);
    }


    // Prints out the statistical variables after performing the simulation
    printf("AP Buffer Size: %d pkts\n", BUFFER_SIZE);
    printf("Wireless Link Speed: %f bps\n", transmissionSpeedBits);
    printf("End Time: %f\n", currentTime);
    printf("Incoming Traffic: %d pkts    %d bytes\n", packetsReceived, bytesReceived);
    printf("Outgoing Traffic: %d pkts    %d bytes\n", packetsSent, bytesSent);
    printf("Buffered Traffic: %d pkts    %d bytes\n", 0, 0);
    printf("Discarded Traffic: %d pkts    %d bytes\n", packetsLost, bytesLost);
    printf("Peak Occupancy: %d pkts    %d bytes\n", maxBufferOccupancyHit, maxBufferSizeHit);
    printf("Lost Traffic: %f%% pkts    %f%% bytes\n", (packetsLost / (double) (packetsReceived)) * 100,
           (bytesLost / (double) (bytesReceived)) * 100);
    printf("Average Occupancy: %f pkts    %f bytes\n", 0.0, 0.0);
    printf("Average Queuing Delay: %f sec\n", 0.0);
    printf("Summary: %f %d %d %d %d %d %d %d %f %f %f %f %f\n", transmissionSpeedBits, BUFFER_SIZE, packetsReceived,
           bytesReceived, packetsSent, bytesSent, packetsLost, bytesLost,
           (packetsLost / (double) (packetsReceived)) * 100, (bytesLost / (double) (bytesReceived)) * 100, 0.0, 0.0,
           0.0);

    return 0;
}
