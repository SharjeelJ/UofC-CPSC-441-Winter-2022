/* This file contains modified code from the following source(s):
 * http://pages.cpsc.ucalgary.ca/~carey/CPSC441/ass4/rand.txt (Professor's exponentially-distributed random number generator)
*/

#include <cstdio>
#include <cstdlib>
#include <cmath>

// Simulation configuration parameters
#define BOZONS 5
#define SLEEP_DURATION 60.0
#define YODEL_DURATION 60.0
#define END_TIME 1000000.000

// Yodel duration model (change to true for yodel duration to be a constant)
#define STANDARDIZED_YODEL_DURATION false

// Bozon states
#define SLEEPING 0
#define YODELING 1

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

// Gets the index for the smallest value (next event) from the passed in array of doubles
int getSmallestValueIndex(const double array[]) {
    int smallestValueIndex = 0;
    for (int counter = 0; counter < BOZONS; counter++)
        if (array[counter] < array[smallestValueIndex])
            smallestValueIndex = counter;
    return smallestValueIndex;
}

// Main function
int main() {
    // Initialize statistical variables for the simulation
    int activeYodlers = 0, latestYodeler = -1, currentBozon, bozonStatuses[BOZONS];
    int yodels = 0, perfectYodels = 0;
    double currentTime = 0.0, latestYodelEndTime = 0.0, eventTimes[BOZONS];
    double idleTime = 0.0, perfectYodelTime = 0.0, melodiousTime = 0.0, screechTime = 0.0;
    bool yodelInterrupted = false;

    // Populates the event times array with an initial random sleep duration for each bozon
    for (int bozon = 0; bozon < BOZONS; bozon++) {
        bozonStatuses[bozon] = SLEEPING;
        eventTimes[bozon] = Exponential(SLEEP_DURATION);
    }

    // Runs the simulation until the next event ends up being after the predefined end time
    while (eventTimes[getSmallestValueIndex(eventTimes)] <= END_TIME) {
        // Stores the bozon that is responsible for the next event (acts as an index to access the event's information)
        currentBozon = getSmallestValueIndex(eventTimes);

        // Declare the variable that will store the time for the next event for the bozon being handled currently
        double newEventTime;

        // Stops the simulation if there are no bozons to simulate
        if (BOZONS == 0) {
            idleTime = END_TIME;
            break;
        }

        // Switch to call the appropriate code segment if the bozon is waking up or going to sleep
        switch (bozonStatuses[currentBozon]) {
            // Handles the case where the bozon is current sleeping and will wake up and begin yodeling
            case SLEEPING:
                // Updates the state of the bozon
                bozonStatuses[currentBozon] = YODELING;

                // Stores the new event time for when the bozon will stop this activity
                if (!STANDARDIZED_YODEL_DURATION)
                    newEventTime = eventTimes[currentBozon] + Exponential(YODEL_DURATION);
                else
                    newEventTime = eventTimes[currentBozon] + YODEL_DURATION;

                // Handles the case where there were no active yodelers before this bozon woke up (meaning the channel was idle)
                if (activeYodlers == 0) {
                    // Sets the interrupted yodel flag to false as this yodel is starting during otherwise idle time
                    yodelInterrupted = false;

                    // Updates the statistical variables
                    idleTime += eventTimes[currentBozon] - currentTime;
                    latestYodelEndTime = newEventTime;
                    currentTime = eventTimes[currentBozon];
                }
                    // Handles the case where there is at least 1 active yodel before this bozon woke up (meaning that it will interrupt a yodel)
                else {
                    // Sets the interrupted yodel flag to true as this yodel is starting during a yodel
                    yodelInterrupted = true;

                    // Checks to see if the current time of the simulation that has been accounted for is not further ahead than the current event, and if so then sets the appropriate statistical variables
                    if (currentTime < eventTimes[currentBozon]) {
                        melodiousTime += eventTimes[currentBozon] - currentTime;
                        currentTime = eventTimes[currentBozon];
                    }

                    // Checks to see if the current bozon's yodel will end before the existing yodel, and if so then records the entire duration of this yodel as screech time and sets the current time to be the end of the yodel (as this time segment has been accounted for)
                    if (latestYodelEndTime > newEventTime && currentTime < newEventTime) {
                        screechTime += newEventTime - currentTime;
                        currentTime = newEventTime;
                    }
                        // If the current bozon's yodel will end after the existing yodel then records the overlapping duration of the two as screech time and sets the current time to be the end of the existing yodel (as this time segment has been accounted for) and stores this yodel as the most recent yodel
                    else if (currentTime < newEventTime) {
                        screechTime += latestYodelEndTime - currentTime;
                        currentTime = latestYodelEndTime;
                        latestYodelEndTime = newEventTime;
                    }
                }

                // Updates the statistical variables
                activeYodlers++;
                latestYodeler = currentBozon;
                eventTimes[currentBozon] = newEventTime;
                break;
                // Handles the case where the bozon is currently yodeling and will stop and go to sleep
            case YODELING:
                // Updates the state of the bozon
                bozonStatuses[currentBozon] = SLEEPING;

                // Stores the new event time for when the bozon will stop this activity
                newEventTime = eventTimes[currentBozon] + Exponential(SLEEP_DURATION);

                // Checks to see if the current yodel was not interrupted (meaning that the latest yodel is from the same bozon who is going to sleep now and there is only 1 active yodeler) meaning that it was a perfect yodel
                if (latestYodeler == currentBozon && activeYodlers == 1 && !yodelInterrupted) {
                    perfectYodels++;
                    perfectYodelTime += eventTimes[currentBozon] - currentTime;
                }

                // Checks to see if the current time of the simulation that has been accounted for is not further ahead than the current event, and if so then sets the appropriate statistical variables
                if (currentTime < eventTimes[currentBozon]) {
                    melodiousTime += eventTimes[currentBozon] - currentTime;
                    currentTime = eventTimes[currentBozon];
                }

                // Updates the statistical variables
                activeYodlers--;
                latestYodeler = -1;
                yodels++;
                eventTimes[currentBozon] = newEventTime;
                break;
        }
    }

    // Prints out the statistical variables after performing the simulation
    printf("M = %i, S = %.3f, Y = %.3f\n", BOZONS, SLEEP_DURATION, YODEL_DURATION);
    printf("Total time observing channel: %.3f\n", END_TIME);
    printf("  Idle time on channel:        %.3f    %.3f%%\n", idleTime, ((idleTime / END_TIME) * 100));
    printf("  Melodious time on channel:   %.3f    %.3f%%\n", melodiousTime, ((melodiousTime / END_TIME) * 100));
    printf("  Screech time on channel:     %.3f    %.3f%%\n", screechTime, ((screechTime / END_TIME) * 100));
    printf("\n");
    printf("  Attempted yodels: %i\n", yodels);
    printf("  Perfect yodels: %i\n", perfectYodels);
    printf("  Perfect yodels/Attempted yodels: %.3f (%.3f%%)\n", ((double) perfectYodels / yodels),
           (((double) perfectYodels / yodels) * 100));
    printf("  Perfect yodel time on the channel: %.3f (%.3f%%)\n", perfectYodelTime,
           ((perfectYodelTime / END_TIME) * 100));

    // Ends the program with the success exit code
    exit(0);
}
