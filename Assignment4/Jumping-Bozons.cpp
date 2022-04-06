/* This file contains modified code from the following source(s):
 * http://pages.cpsc.ucalgary.ca/~carey/CPSC441/ass4/rand.txt (Professor's exponentially-distributed random number generator)
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <vector>
#include <tuple>
#include <algorithm>
#include <queue>

// Constants
#define BOZONS 8
#define SLEEP_DURATION 200.0
#define YODEL_DURATION 10.0

// Simulation parameters
#define END_TIME 1000000.000

// Bozon states
#define SLEEPING 0
#define YODELING 1

// Yodel duration model
#define EXPONENTIAL 1
//#define CONSTANT 1

// Parameters for random number generation (maximum positive integer 2^31 - 1)
#define MAX_INT 2147483647

// Generate a random floating point number uniformly distributed in [0,1]
double Uniform01() {
    double randnum;
    // get a random positive integer from random()
//    srandom(time(NULL));
    randnum = (double) 1.0 * rand();

    // divide by max int to get something in the range 0.0 to 1.0
    randnum = randnum / (1.0 * MAX_INT);
    return (randnum);
}

// Generate a random floating point number from an exponential distribution with mean mu
double Exponential(double mu) {
    double randnum, ans;

    randnum = Uniform01();
    ans = -(mu) * log(randnum);
    return (ans);
}

// Custom comparator that will be used to sort the events vector based on the time of the event in descending order
bool customComparator(std::tuple<double, double> const &event1, std::tuple<double, double> const &event2) {
    if (std::get<0>(event1) == std::get<0>(event2))
        return std::get<1>(event1) > std::get<1>(event2);
    else
        return std::get<0>(event1) > std::get<0>(event2);
}

int main() {
//    int i;
//    int active, nextEventIndex, status[BOZONS];
    int yodelTries = 0, perfectYodels = 0;
//    int mostRecentYodelerToStart;
//    double mostRecentYodelStartTime, mostRecentYodelEndTime;
    double idleTime = 0.0, perfectYodelTime = 0.0, melodiousTime = 0.0, screechTime = 0.0;

    // Vector that will store all the events
    std::vector<std::tuple<double, double>> eventsVector;

    for (int currentBozon = 1; currentBozon <= BOZONS; currentBozon++) {
        double latestEventEnd = 0.0;
        while (latestEventEnd < END_TIME) {
            double sleepDuration = Exponential(SLEEP_DURATION);
            double yodelDuration = Exponential(YODEL_DURATION);

            if (latestEventEnd + sleepDuration + yodelDuration > END_TIME)
                break;

            eventsVector.emplace_back(
                    std::make_tuple(latestEventEnd + sleepDuration, latestEventEnd + sleepDuration + yodelDuration));

            latestEventEnd += sleepDuration + yodelDuration;
        }
    }

    // Sorts the vector containing all the events in the simulation in descending order based on the time of each event
    std::sort(eventsVector.begin(), eventsVector.end(), &customComparator);

    // Moves all the events from the vector to a queue
    std::queue<std::tuple<double, double>> eventsQueue;

    while (!eventsVector.empty()) {
        eventsQueue.push(std::make_tuple(std::get<0>(eventsVector.back()), std::get<1>(eventsVector.back())));
        eventsVector.pop_back();
    }

    yodelTries = eventsQueue.size();

    double currentYodelStartTime = 0.0;
    double currentYodelEndTime = 0.0;
    double timeAccountedTo = 0.0;
    bool currentYodelInterrupted = false;

    while (!eventsQueue.empty()) {
        double nextYodelStartTime = std::get<0>(eventsQueue.front());
        double nextYodelEndTime = std::get<1>(eventsQueue.front());

        if (currentYodelStartTime == 0 && currentYodelEndTime == 0) {
            currentYodelStartTime = nextYodelStartTime;
            currentYodelEndTime = nextYodelEndTime;
            idleTime += nextYodelStartTime;
            timeAccountedTo = nextYodelStartTime;
            eventsQueue.pop();
            nextYodelStartTime = std::get<0>(eventsQueue.front());
            nextYodelEndTime = std::get<1>(eventsQueue.front());
        }

        if (currentYodelEndTime <= nextYodelStartTime) {
            melodiousTime += currentYodelEndTime - timeAccountedTo;
            if (!currentYodelInterrupted) {
                perfectYodels++;
                perfectYodelTime += currentYodelEndTime - timeAccountedTo;
            }
            currentYodelInterrupted = false;
            idleTime += nextYodelStartTime - currentYodelEndTime;
            currentYodelStartTime = nextYodelStartTime;
            currentYodelEndTime = nextYodelEndTime;
            timeAccountedTo = currentYodelStartTime;
            eventsQueue.pop();
        } else {
            currentYodelInterrupted = true;
            melodiousTime += nextYodelStartTime - timeAccountedTo;
            timeAccountedTo = nextYodelStartTime;

//            nextYodelStartTime = std::get<0>(eventsQueue.front());
//            nextYodelEndTime = std::get<1>(eventsQueue.front());
//            eventsQueue.pop();

            if (currentYodelEndTime > nextYodelEndTime) {
                while (currentYodelEndTime > nextYodelEndTime && !eventsQueue.empty()) {
                    screechTime += nextYodelEndTime - timeAccountedTo;
                    timeAccountedTo = nextYodelEndTime;

                    eventsQueue.pop();
                    nextYodelStartTime = std::get<0>(eventsQueue.front());
                    nextYodelEndTime = std::get<1>(eventsQueue.front());
                }
                melodiousTime += nextYodelStartTime - timeAccountedTo;
                timeAccountedTo = nextYodelStartTime;

                screechTime += currentYodelEndTime - timeAccountedTo;
                timeAccountedTo = currentYodelEndTime;
            } else {
                screechTime += currentYodelEndTime - timeAccountedTo;
                timeAccountedTo = currentYodelEndTime;
            }

            currentYodelStartTime = nextYodelStartTime;
            currentYodelEndTime = nextYodelEndTime;
            eventsQueue.pop();
        }
    }

    if (!currentYodelInterrupted) {
        perfectYodels++;
        perfectYodelTime += currentYodelEndTime - timeAccountedTo;
    } else {
        melodiousTime += currentYodelEndTime - timeAccountedTo;
    }

    // Prints out the statistical variables after performing the simulation
    printf("M = %i, S = %f, Y = %f\n", BOZONS, YODEL_DURATION, SLEEP_DURATION);
    printf("Total time observing channel: %f\n", END_TIME);
    printf("  Idle time on channel: %f    %f%%\n", idleTime, (idleTime / END_TIME) * 100);
    printf("  Melodious time on channel: %f    %f%%\n", melodiousTime, ((melodiousTime / END_TIME) * 100));
    printf("  Screech time on channel: %f    %f%%\n", screechTime, ((screechTime / END_TIME) * 100));
    printf("\n");
    printf("  Attempted yodels: %i\n", yodelTries);
    printf("  Perfect yodels: %i\n", perfectYodels);
    printf("  Perfect yodels/Attempted yodels: %f (%f%%)\n", ((double) perfectYodels / yodelTries),
           (((double) perfectYodels / yodelTries) * 100));
    printf("  Perfect yodel time on the channel: %f (%f%%)\n", perfectYodelTime, ((perfectYodelTime / END_TIME) * 100));

    // Ends the program with the success exit code
    exit(0);
}
