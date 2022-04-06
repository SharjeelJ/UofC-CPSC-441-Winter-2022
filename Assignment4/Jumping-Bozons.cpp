/* This file contains modified code from the following source(s):
 * http://pages.cpsc.ucalgary.ca/~carey/CPSC441/ass4/rand.txt (Professor's exponentially-distributed random number generator)
*/

#include <cstdio>
#include <cstdlib>
#include <cmath>

// Constants
#define BOZONS 5
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
    int activeYodlers = 0, bozon = 0, bozonStatus[BOZONS];
    double eventEndTime[BOZONS];
    int yodels = 0, perfectYodels = 0;
    int latestYodeler = -1;
    double latestYodelStartTime = 0.0, latestYodelEndTime = 0.0;
    bool yodelInterrupted = false;
    double idleTime = 0.0, perfectYodelTime = 0.0, melodiousTime = 0.0, screechTime = 0.0;
    double currentTime = 0.0;

    for (int currentBozon = 0; currentBozon < BOZONS; currentBozon++) {
        bozonStatus[currentBozon] = SLEEPING;
        eventEndTime[currentBozon] = Exponential(SLEEP_DURATION);
    }

//    while (currentTime <= END_TIME) {
    while (eventEndTime[getSmallestValueIndex(eventEndTime)] <= END_TIME) {
        bozon = getSmallestValueIndex(eventEndTime);
        double newEventEndTime;

        if (BOZONS == 0) {
            idleTime = END_TIME;
            break;
        }

        switch (bozonStatus[bozon]) {
            case SLEEPING:
                bozonStatus[bozon] = YODELING;
                newEventEndTime = eventEndTime[bozon] + Exponential(YODEL_DURATION);

                if (activeYodlers == 0) {
                    yodelInterrupted = false;
                    activeYodlers++;
                    idleTime += eventEndTime[bozon] - currentTime;
                    latestYodelStartTime = eventEndTime[bozon];
                    latestYodelEndTime = newEventEndTime;
                    currentTime = eventEndTime[bozon];
                } else {
                    yodelInterrupted = true;
                    activeYodlers++;

                    if (currentTime < eventEndTime[bozon]) {
                        melodiousTime += eventEndTime[bozon] - currentTime;
                        currentTime = eventEndTime[bozon];
                    }

                    if (latestYodelEndTime > newEventEndTime) {
                        screechTime += newEventEndTime - currentTime;
                        currentTime = newEventEndTime;
                    } else {
                        screechTime += latestYodelEndTime - currentTime;
                        currentTime = latestYodelEndTime;
                        latestYodelStartTime = eventEndTime[bozon];
                        latestYodelEndTime = newEventEndTime;
                    }
                }

                latestYodeler = bozon;

                eventEndTime[bozon] = newEventEndTime;
                break;
            case YODELING:
                newEventEndTime = eventEndTime[bozon] + Exponential(SLEEP_DURATION);
                if (latestYodeler == bozon && activeYodlers == 1 && !yodelInterrupted) {
                    bozonStatus[bozon] = SLEEPING;
                    activeYodlers--;
                    yodels++;
                    perfectYodels++;

                    perfectYodelTime += eventEndTime[bozon] - currentTime;
                    melodiousTime += eventEndTime[bozon] - currentTime;

                    if (currentTime < eventEndTime[bozon])
                        currentTime = eventEndTime[bozon];

                    eventEndTime[bozon] = newEventEndTime;
                } else {
                    bozonStatus[bozon] = SLEEPING;
                    activeYodlers--;
                    yodels++;

                    if (currentTime < eventEndTime[bozon]) {
                        melodiousTime += eventEndTime[bozon] - currentTime;
                        currentTime = eventEndTime[bozon];
                    }

                    eventEndTime[bozon] = newEventEndTime;
                }
                break;
        }
    }

    // Prints out the statistical variables after performing the simulation
    printf("M = %i, S = %f, Y = %f\n", BOZONS, YODEL_DURATION, SLEEP_DURATION);
    printf("Total time observing channel: %f\n", END_TIME);
    printf("  Idle time on channel: %f    %f%%\n", idleTime, (idleTime / END_TIME) * 100);
    printf("  Melodious time on channel: %f    %f%%\n", melodiousTime, ((melodiousTime / END_TIME) * 100));
    printf("  Screech time on channel: %f    %f%%\n", screechTime, ((screechTime / END_TIME) * 100));
    printf("\n");
    printf("  Attempted yodels: %i\n", yodels);
    printf("  Perfect yodels: %i\n", perfectYodels);
    printf("  Perfect yodels/Attempted yodels: %f (%f%%)\n", ((double) perfectYodels / yodels),
           (((double) perfectYodels / yodels) * 100));
    printf("  Perfect yodel time on the channel: %f (%f%%)\n", perfectYodelTime,
           ((perfectYodelTime / END_TIME) * 100));

    // Ends the program with the success exit code
    exit(0);
}
