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
#define TRANSMISSION_SPEED_MEGABITS 5.0

// File path of the trace file being simulated
#define TRACE_FILE "soccer.txt"

// File path of the bonus trace file being simulated
#define TRACE_FILE_BONUS "starwars.txt"

// Boolean to toggle the bonus component's calculation on/off (if turned on then performs the simulation on packet data from both the trace files, otherwise only uses one trace file)
#define BONUS false

// Event types
#define PACKET_ARRIVAL 0
#define PACKET_DEPARTURE 1

// Custom comparator that will be used to sort the events vector based on the time of the event in descending order (used for the bonus)
bool customComparator(std::tuple<double, int> const &event1, std::tuple<double, int> const &event2) {
    return std::get<0>(event1) > std::get<0>(event2);
}

// Main function
int main() {
    // Computes and stores the router's transmission speed with different units
    double transmissionSpeedBits = TRANSMISSION_SPEED_MEGABITS * 1000000.0;
    double transmissionSpeedBytes = TRANSMISSION_SPEED_MEGABITS * 125000.0;

    // Queues to store all the events in the simulation (each event is of the form <Time of Event, Byte Size>)
    std::queue<std::tuple<double, int>> packetArrivalQueue;
    std::queue<std::tuple<double, int>> packetDepartureQueue;

    // Checks to see if the bonus flag has not been enabled
    if (!BONUS) {
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
    } else {
        // Vector that will store all the packet arrival events from both the trace files
        std::vector<std::tuple<double, int>> packetArrivalVector;

        // Initialize variables that will be used for both the trace files' IO
        FILE *filePointer1, *filePointer2;
        char filePath1[] = TRACE_FILE, filePath2[] = TRACE_FILE_BONUS;
        char fileCurrentLine1[100], fileCurrentLine2[100];

        // Opens the first trace file otherwise prints an error and quits
        if ((filePointer1 = fopen(filePath1, "r")) == nullptr) {
            printf("Cannot open the first trace file!");
            exit(1);
        }

        // Populates the packet arrival vector with the contents of the entire first trace file
        while (fgets(fileCurrentLine1, 100, filePointer1) != nullptr) {
            double packetArrivalTime = atof(strtok(fileCurrentLine1, " "));
            int packetSize = atoi(strtok(nullptr, " "));
            packetArrivalVector.emplace_back(std::make_tuple(packetArrivalTime, packetSize));
        }

        // Closes the first trace file's IO
        fclose(filePointer1);

        // Opens the second trace file otherwise prints an error and quits
        if ((filePointer2 = fopen(filePath2, "r")) == nullptr) {
            printf("Cannot open the second trace file!");
            exit(1);
        }

        // Populates the packet arrival vector with the contents of the entire second trace file
        while (fgets(fileCurrentLine2, 100, filePointer2) != nullptr) {
            double packetArrivalTime = atof(strtok(fileCurrentLine2, " "));
            int packetSize = atoi(strtok(nullptr, " "));
            packetArrivalVector.emplace_back(std::make_tuple(packetArrivalTime, packetSize));
        }

        // Closes the second trace file's IO
        fclose(filePointer2);

        // Sorts the vector containing all the packet arrival events in the simulation in descending order based on the time of each event
        std::sort(packetArrivalVector.begin(), packetArrivalVector.end(), &customComparator);

        // Moves all the events from the vector to the queue
        while (!packetArrivalVector.empty()) {
            packetArrivalQueue.push(std::make_tuple(std::get<0>(packetArrivalVector.back()),
                                                    std::get<1>(packetArrivalVector.back())));
            packetArrivalVector.pop_back();
        }
    }

    // Initialize statistical variables for the simulation
    long packetsReceived = 0, packetsBuffered = 0, packetsSent = 0, packetsLost = 0;
    long bytesReceived = 0, bytesBuffered = 0, bytesSent = 0, bytesLost = 0;
    long currentBufferOccupancy = 0, currentBufferSize = 0;
    long maxBufferOccupancyHit = 0, maxBufferSizeHit = 0;
    long cumulativeBufferOccupancy = 0, cumulativeBufferSize = 0;
    double cumulativeDelay = 0.0;

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
                    packetsBuffered++;
                    bytesBuffered += currentEventSize;
                    cumulativeDelay += (currentTime + (currentBufferSize / transmissionSpeedBytes)) -
                                       (currentTime + (currentEventSize / transmissionSpeedBytes));
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
                cumulativeBufferOccupancy += currentBufferOccupancy;
                cumulativeBufferSize += currentBufferSize;
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
    printf("Incoming Traffic: %ld pkts    %ld bytes\n", packetsReceived, bytesReceived);
    printf("Outgoing Traffic: %ld pkts    %ld bytes\n", packetsSent, bytesSent);
    printf("Buffered Traffic: %ld pkts    %ld bytes\n", packetsBuffered, bytesBuffered);
    printf("Discarded Traffic: %ld pkts    %ld bytes\n", packetsLost, bytesLost);
    printf("Peak Occupancy: %ld pkts    %ld bytes\n", maxBufferOccupancyHit, maxBufferSizeHit);
    printf("Lost Traffic: %f%% pkts    %f%% bytes\n", (packetsLost / (double) (packetsReceived)) * 100,
           (bytesLost / (double) (bytesReceived)) * 100);
    printf("Average Occupancy: %f pkts    %f KB\n", cumulativeBufferOccupancy / (double) packetsBuffered,
           (cumulativeBufferSize / (double) packetsBuffered) / 1000);
    printf("Average Queuing Delay: %f sec\n", cumulativeDelay / packetsBuffered);
    printf("Summary: %f %d %ld %ld %ld %ld %ld %ld %f %f %f %f %f\n", transmissionSpeedBits, BUFFER_SIZE,
           packetsReceived, bytesReceived, packetsSent, bytesSent, packetsLost, bytesLost,
           (packetsLost / (double) (packetsReceived)) * 100, (bytesLost / (double) (bytesReceived)) * 100,
           cumulativeBufferOccupancy / (double) packetsBuffered,
           (cumulativeBufferSize / (double) packetsBuffered) / 1000,
           cumulativeDelay / packetsBuffered);

    // Ends the program with the success exit code
    exit(0);
}
