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
#define BOZONS 2
#define SLEEP_DURATION 60.0
#define YODEL_DURATION 60.0

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

int getSmallestValueIndex(double array[]) {
    int smallestValueIndex = 0;
    for (int counter = 0; counter < BOZONS; counter++)
        if (array[counter] < array[smallestValueIndex])
            smallestValueIndex = counter;
    return smallestValueIndex;
}

int main() {
    int simulationState = SLEEPING, bozon = 0, bozonStatus[BOZONS];
    double eventEndTime[BOZONS];
    int yodels = 0, perfectYodels = 0;
    int latestYodeler = -1;
    double latestYodelStartTime = -1.0, latestYodelEndTime = -1.0;
    double idleTime = 0.0, perfectYodelTime = 0.0, melodiousTime = 0.0, screechTime = 0.0;
    double currentTime = 0.0;

    for (int currentBozon = 0; currentBozon < BOZONS; currentBozon++) {
        bozonStatus[currentBozon] = SLEEPING;
        eventEndTime[currentBozon] = Exponential(SLEEP_DURATION);
    }

    while (currentTime <= END_TIME) {
        bozon = getSmallestValueIndex(eventEndTime);

        switch (bozonStatus[bozon]) {
            case SLEEPING:
                simulationState = YODELING;
                bozonStatus[bozon] = YODELING;

                if (simulationState == SLEEPING)
                    idleTime += eventEndTime[bozon] - currentTime;

                latestYodeler = bozon;
                latestYodelStartTime = eventEndTime[bozon];
                currentTime = eventEndTime[bozon];
                eventEndTime[bozon] += Exponential(YODEL_DURATION);
                latestYodelEndTime = eventEndTime[bozon];
                break;
            case YODELING:
                if (latestYodeler == bozon) {
                    simulationState = SLEEPING;
                    bozonStatus[bozon] = SLEEPING;

                    perfectYodels++;
                    perfectYodelTime += eventEndTime[bozon] - currentTime;

                    currentTime = eventEndTime[bozon];
                    eventEndTime[bozon] += Exponential(SLEEP_DURATION);
                } else {
                    bozonStatus[bozon] = SLEEPING;

                    yodels++;
                    melodiousTime += eventEndTime[bozon] - currentTime;

                    currentTime = eventEndTime[bozon];
                    eventEndTime[bozon] += Exponential(SLEEP_DURATION);
                }
                break;
        }

//        if (simulationState == SLEEPING) {
//            idleTime += eventEndTime[bozon] - currentTime;
//            bozonStatus[bozon] = YODELING;
//            currentTime = eventEndTime[bozon];
//            latestYodelStartTime = eventEndTime[bozon];
//            eventEndTime[bozon] += Exponential(YODEL_DURATION);
//            latestYodelEndTime = eventEndTime[bozon];
//            simulationState = YODELING;
//            latestYodeler = bozon;
//        } else {
//            if (bozon == latestYodeler) {
//                perfectYodels++;
//                perfectYodelTime += eventEndTime[bozon] - currentTime;
//                latestYodeler = -1;
//                latestYodelStartTime = -1.0;
//                latestYodelEndTime = -1.0;
//                bozonStatus[bozon] = SLEEPING;
//                simulationState = SLEEPING;
//                currentTime = eventEndTime[bozon];
//                eventEndTime[bozon] = Exponential(SLEEP_DURATION);
//            } else {
//            }
//
//            melodiousTime += eventEndTime[bozon] - currentTime;
//            currentTime = eventEndTime[bozon];
//        }
//
//
//        double nextYodelStartTime = std::get<0>(eventsQueue.front());
//        double nextYodelEndTime = std::get<1>(eventsQueue.front());
//
//        if (currentEventStartTime == 0 && currentEventEndTime == 0) {
//            currentEventStartTime = nextYodelStartTime;
//            currentEventEndTime = nextYodelEndTime;
//            idleTime += nextYodelStartTime;
//            currentTime = nextYodelStartTime;
//            eventsQueue.pop();
//            nextYodelStartTime = std::get<0>(eventsQueue.front());
//            nextYodelEndTime = std::get<1>(eventsQueue.front());
//        }
//
//        if (currentEventEndTime <= nextYodelStartTime) {
//            melodiousTime += currentEventEndTime - currentTime;
//            if (!currentYodelInterrupted) {
//                perfectYodels++;
//                perfectYodelTime += currentEventEndTime - currentTime;
//            }
//            currentYodelInterrupted = false;
//            idleTime += nextYodelStartTime - currentEventEndTime;
//            currentEventStartTime = nextYodelStartTime;
//            currentEventEndTime = nextYodelEndTime;
//            currentTime = currentEventStartTime;
//            eventsQueue.pop();
//        } else {
//            currentYodelInterrupted = true;
//            melodiousTime += nextYodelStartTime - currentTime;
//            currentTime = nextYodelStartTime;
//
////            nextYodelStartTime = std::get<0>(eventsQueue.front());
////            nextYodelEndTime = std::get<1>(eventsQueue.front());
////            eventsQueue.pop();
//
//            if (currentEventEndTime > nextYodelEndTime) {
//                while (currentEventEndTime > nextYodelEndTime && !eventsQueue.empty()) {
//                    screechTime += nextYodelEndTime - currentTime;
//                    currentTime = nextYodelEndTime;
//
//                    eventsQueue.pop();
//                    nextYodelStartTime = std::get<0>(eventsQueue.front());
//                    nextYodelEndTime = std::get<1>(eventsQueue.front());
//                }
//                melodiousTime += nextYodelStartTime - currentTime;
//                currentTime = nextYodelStartTime;
//
//                screechTime += currentEventEndTime - currentTime;
//                currentTime = currentEventEndTime;
//            } else {
//                screechTime += currentEventEndTime - currentTime;
//                currentTime = currentEventEndTime;
//            }
//
//            currentEventStartTime = nextYodelStartTime;
//            currentEventEndTime = nextYodelEndTime;
//            eventsQueue.pop();
//        }
//    }
//
//    if (!currentYodelInterrupted) {
//        perfectYodels++;
//        perfectYodelTime += currentEventEndTime - currentTime;
//    } else {
//        melodiousTime += currentEventEndTime - currentTime;
    }

    // Prints out the statistical variables after performing the simulation
    printf("M = %i, S = %f, Y = %f\n", BOZONS, YODEL_DURATION, SLEEP_DURATION);
    printf("Total eventEndTime observing channel: %f\n", END_TIME);
    printf("  Idle eventEndTime on channel: %f    %f%%\n", idleTime, (idleTime / END_TIME) * 100);
    printf("  Melodious eventEndTime on channel: %f    %f%%\n", melodiousTime, ((melodiousTime / END_TIME) * 100));
    printf("  Screech eventEndTime on channel: %f    %f%%\n", screechTime, ((screechTime / END_TIME) * 100));
    printf("\n");
    printf("  Attempted yodels: %i\n", yodels);
    printf("  Perfect yodels: %i\n", perfectYodels);
    printf("  Perfect yodels/Attempted yodels: %f (%f%%)\n", ((double) perfectYodels / yodels),
           (((double) perfectYodels / yodels) * 100));
    printf("  Perfect yodel eventEndTime on the channel: %f (%f%%)\n", perfectYodelTime,
           ((perfectYodelTime / END_TIME) * 100));

    // Ends the program with the success exit code
    exit(0);
}
