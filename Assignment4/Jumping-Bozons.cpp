/* This file contains modified code from the following source(s):
 * http://pages.cpsc.ucalgary.ca/~carey/CPSC441/ass4/rand.txt (Professor's exponentially-distributed random number generator)
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// Constants
#define BOZONS 1
#define SLEEP_DURATION 60.0
#define YODEL_DURATION 60.0

// Simulation parameters
#define WARM_UP 0.0
#define END_TIME 1000.0

// Bozon states
#define SLEEPING 0
#define YODELING 1

// Yodel duration model
#define EXPONENTIAL 1
//#define CONSTANT 1

// Parameters for random number generation (maximum positive integer 2^31 - 1)
#define MAX_INT 2147483647

double log();

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

int main() {
    int i;
    int active, nextEventIndex, status[BOZONS];
    int yodelTries, perfectYodels;
    int mostRecentYodelerToStart;
    double mostRecentYodelStartTime, mostRecentYodelEndTime;

    printf("Test 1: %f\n\n", Exponential(SLEEP_DURATION));
    printf("Test 2: %f\n\n", Exponential(YODEL_DURATION));

    // Prints out the statistical variables after performing the simulation
    printf("M = %i, S = %f, Y = %f\n", BOZONS, YODEL_DURATION, SLEEP_DURATION);
    printf("Total time observing channel: %f\n", END_TIME);
    printf("  Melodious time on channel: %f    %f%%\n", 0.0, 0.0);
    printf("  Screech time on channel: %f    %f%%\n", 0.0, 0.0);
    printf("\n");
    printf("  Attempted yodels: %i\n", yodelTries);
    printf("  Perfect yodels: %i\n", perfectYodels);
    printf("  Perfect yodels/Attempted yodels: %f (%f%%)\n", ((double) perfectYodels / yodelTries),
           ((double) perfectYodels / yodelTries) * 100);
    printf("  Perfect yodel time on the channel: %f (%f%%)\n", 0.0, 0.0);

    // Ends the program with the success exit code
    exit(0);
}
