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

int main() {
    // Computes and stores the router's transmission speed with different units
    double transmissionSpeedBits = TRANSMISSION_SPEED_MEGABITS * 1000000.0;
    double transmissionSpeedBytes = transmissionSpeedBits / 8.0;

    // Initialize variables that will be used for the trace file's IO
    FILE *filePointer;
    char filePath[] = TRACE_FILE;
    char fileCurrentLine[100];

    // Opens the trace file otherwise prints an error and quits
    if ((filePointer = fopen(filePath, "r")) == nullptr) {
        printf("Cannot open trace file!");
        exit(1);
    }

    // Queue to store the packets from the trace file
    std::queue<std::tuple<double, int>> packets;

    // Populates the queue with the contents of the entire trace file
    while (fgets(fileCurrentLine, 100, filePointer) != nullptr) {
        // Gets and stores the packet's arrival time and size (in bytes)
        double packetArrivalTime = atof(strtok(fileCurrentLine, " "));
        int packetSize = atoi(strtok(nullptr, " "));
        packets.push(std::make_tuple(packetArrivalTime, packetSize));
    }

    // Closes the trace file's IO
    fclose(filePointer);

    // Initialize statistical variables for the simulation
    int packetsReceived = 0, packetsSent = 0, packetsLost = 0;
    int bytesReceived = 0, bytesSent = 0, bytesLost = 0;
    int currentBufferSize = 0;
    int maxBufferOccupancyHit = 0, maxBufferSizeHit = 0;
    double cumulativeDelay = 0;

    // Initialize state variables for the simulation
    double currentTime = 0.0;

    // Queue to simulate the router's buffer for storing packets
    std::queue<std::tuple<double, int>> buffer;

    // Loops through all the stored packets (performs the simulation)
    while (!packets.empty() || !buffer.empty()) {
        // Checks to see if there still are packets that will arrive in the future
        if (!packets.empty()) {
            // Stores the arrival time and size of the next packet
            double packetArrivalTime = std::get<0>(packets.front());
            int packetSize = std::get<1>(packets.front());

            // Checks to see if the buffer size of the router is 0 (means that all packets received will be lost)
            if (BUFFER_SIZE == 0) {
                currentTime = packetArrivalTime;
                packets.pop();
                packetsReceived++;
                bytesReceived += packetSize;
                packetsLost++;
                bytesLost += packetSize;
            }
                // Checks to see if the router buffer is empty
            else if (buffer.empty()) {
                buffer.push(std::make_tuple(packetArrivalTime, packetSize));
                packets.pop();
                packetsReceived++;
                bytesReceived += packetSize;
                currentBufferSize += packetSize;
            }
                // Run if there is a packet to arrive and the buffer is not empty
            else {
                // Stores how long we have until the next packet will arrive and how long it will take for the oldest packet in the buffer to be sent
                double nextPacketArrivalTimeDelta = packetArrivalTime - currentTime;
                double currentPacketSendTimeDelta =
                        (std::get<1>(buffer.front()) / transmissionSpeedBytes) + std::get<0>(buffer.front()) -
                        currentTime;

                // Checks to see if the oldest packet in the buffer will take longer to send than the packet that is to arrive
                if (nextPacketArrivalTimeDelta < currentPacketSendTimeDelta && buffer.size() < BUFFER_SIZE) {
                    buffer.push(std::make_tuple(packetArrivalTime, packetSize));
                    packets.pop();
                    packetsReceived++;
                    bytesReceived += packetSize;
                    currentBufferSize += packetSize;
                }
                    // Checks to see if the oldest packet in the buffer will take less time to send than the packet that is to arrive
                else if (nextPacketArrivalTimeDelta >= currentPacketSendTimeDelta) {
                    packetsSent++;
                    bytesSent += std::get<1>(buffer.front());
                    currentBufferSize -= std::get<1>(buffer.front());
                    buffer.pop();
                    buffer.push(std::make_tuple(packetArrivalTime, packetSize));
                    currentTime += (std::get<1>(buffer.front()) / transmissionSpeedBytes) + nextPacketArrivalTimeDelta;
                    packets.pop();
                    packetsReceived++;
                    bytesReceived += packetSize;
                    currentBufferSize += packetSize;
                } else {
                    packets.pop();
                    packetsLost++;
                    bytesLost += packetSize;
                }
            }
        }
            // Run if there are no packets that will arrive in the future
        else {
            while (!buffer.empty()) {
                currentTime += (std::get<1>(buffer.front()) / transmissionSpeedBytes);
                packetsSent++;
                bytesSent += std::get<1>(buffer.front());
                currentBufferSize -= std::get<1>(buffer.front());
                buffer.pop();
            }
        }

        // Checks to see if the record number of bytes stored in the buffer has been exceeded
        if (currentBufferSize > maxBufferSizeHit)
            maxBufferSizeHit = currentBufferSize;

        // Checks to see if the record number of packets stored in the buffer has been exceeded
        if (buffer.size() > maxBufferOccupancyHit)
            maxBufferOccupancyHit = buffer.size();
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
    printf("Summary: %f %d %d %d %d %d %d %d %f %f %f %f %f\n", transmissionSpeedBits, BUFFER_SIZE,
           packetsReceived, bytesReceived, packetsSent, bytesSent, packetsLost, bytesLost,
           (packetsLost / (double) (packetsReceived)) * 100, (bytesLost / (double) (bytesReceived)) * 100, 0.0, 0.0,
           0.0);

    return 0;
}
