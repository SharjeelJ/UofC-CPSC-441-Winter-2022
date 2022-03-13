/* This file contains modified code from the following source(s):
 * 7-Network-Simulation.pdf from T02/T03
*/

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <queue>
#include <tuple>

// Router buffer size (maximum number of queued packets that can be held)
#define BUFFER_SIZE 100

// Router transmission speed in megabits per second (mbps)
#define TRANSMISSION_SPEED_MEGABITS 3.0

// File path of the trace file being simulated
#define TRACE_FILE "starwars.txt"

// Event types
#define PACKET_ARRIVAL 0
#define PACKET_DEPARTURE 1

// Main function
int main() {
    // Computes and stores the router's transmission speed with different units
    double transmissionSpeedBits = TRANSMISSION_SPEED_MEGABITS * 1000000.0;
    double transmissionSpeedBytes = TRANSMISSION_SPEED_MEGABITS * 125000.0;

    // Queues to store all the events in the simulation (each event is of the form <Time of Event, Byte Size>)
    std::queue<std::tuple<double, int>> packetArrivalQueue;
    std::queue<std::tuple<double, int>> packetDepartureQueue;

    // Initialize variables that will be used for the trace file's IO
    FILE *filePointer;
    char filePath[] = TRACE_FILE;
    char fileCurrentLine[100];

    // Opens the trace file otherwise prints an error and quits
    if ((filePointer = fopen(filePath, "r")) == nullptr) {
        printf("Cannot open trace file!");
        exit(1);
    }

    // Populates the packet arrival queue with the contents of the entire trace file
    while (fgets(fileCurrentLine, 100, filePointer) != nullptr) {
        double packetArrivalTime = atof(strtok(fileCurrentLine, " "));
        int packetSize = atoi(strtok(nullptr, " "));
        packetArrivalQueue.push(std::make_tuple(packetArrivalTime, packetSize));
    }

    // Closes the trace file's IO
    fclose(filePointer);

    // Initialize statistical variables for the simulation
    int packetsReceived = 0, packetsSent = 0, packetsLost = 0;
    int bytesReceived = 0, bytesSent = 0, bytesLost = 0;
    int currentBufferOccupancy = 0, currentBufferSize = 0;
    int maxBufferOccupancyHit = 0, maxBufferSizeHit = 0;
    double cumulativeDelay = 0;

    // Initialize state variables for the simulation
    double currentTime = 0.0;

    // Loops through all the stored events (performs the simulation)
    while (!packetArrivalQueue.empty() || !packetDepartureQueue.empty()) {
        // Initialize variables that will be updated to store the information relating to the current event being simulated
        double currentEventTime;
        int currentEventSize;
        int currentEventType;

        // Checks to see if both the packet arrival queue and packet departure queue have an entry
        if (!packetArrivalQueue.empty() && !packetDepartureQueue.empty())
            // Checks to see if the packet arrival queue's entry occurs before the packet departure queue's entry
            if (std::get<0>(packetArrivalQueue.front()) < std::get<0>(packetDepartureQueue.front())) {
                currentEventTime = std::get<0>(packetArrivalQueue.front());
                currentEventSize = std::get<1>(packetArrivalQueue.front());
                currentEventType = PACKET_ARRIVAL;
            } else {
                currentEventTime = std::get<0>(packetDepartureQueue.front());
                currentEventSize = std::get<1>(packetDepartureQueue.front());
                currentEventType = PACKET_DEPARTURE;
            }
            // Checks to see if the packet arrival queue has an entry but the packet departure queue is empty
        else if (!packetArrivalQueue.empty() && packetDepartureQueue.empty()) {
            currentEventTime = std::get<0>(packetArrivalQueue.front());
            currentEventSize = std::get<1>(packetArrivalQueue.front());
            currentEventType = PACKET_ARRIVAL;
        } else {
            currentEventTime = std::get<0>(packetDepartureQueue.front());
            currentEventSize = std::get<1>(packetDepartureQueue.front());
            currentEventType = PACKET_DEPARTURE;
        }

        // Updates the current time of the simulation to be the current event's time
        currentTime = currentEventTime;

        // Switch to call the appropriate code based on whether we are handling a packet's arrival or departure event
        switch (currentEventType) {
            // Run if the current event is a packet arrival
            case PACKET_ARRIVAL:
                packetsReceived++;
                bytesReceived += currentEventSize;

                // Checks to see if the buffer is full
                if (currentBufferOccupancy >= BUFFER_SIZE) {
                    // Updates the statistical variables and the queue
                    packetsLost++;
                    bytesLost += currentEventSize;
                    packetArrivalQueue.pop();
                } else {
                    // Updates the statistical variables and the queues
                    currentBufferOccupancy++;
                    currentBufferSize += currentEventSize;
                    packetArrivalQueue.pop();
                    packetDepartureQueue.push(
                            std::make_tuple(currentTime + (currentBufferSize / transmissionSpeedBytes),
                                            currentEventSize));

                    // Checks to see if the record number of packets stored in the buffer has been exceeded
                    if (currentBufferOccupancy > maxBufferOccupancyHit)
                        maxBufferOccupancyHit = currentBufferOccupancy;

                    // Checks to see if the record number of bytes stored in the buffer has been exceeded
                    if (currentBufferSize > maxBufferSizeHit)
                        maxBufferSizeHit = currentBufferSize;
                }
                break;
                // Run if the current event is a packet departure
            case PACKET_DEPARTURE:
                // Updates the statistical variables and the queue
                packetsSent++;
                bytesSent += currentEventSize;
                currentBufferOccupancy--;
                currentBufferSize -= currentEventSize;
                packetDepartureQueue.pop();
                break;
        }
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
