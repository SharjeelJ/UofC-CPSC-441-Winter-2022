# Assignment 3: Router Buffering

### Instructions

1) Download Router-Buffering.cpp , soccer.txt , zoom.txt , and starwars.txt and place them all into a single new folder
2) Open Router-Buffering.cpp in a text editor and edit line 13 (buffer size), 16 (transmission speed in mbps), 19 (trace
   file path) and 25 (boolean to enable/disable the bonus component) as desired
3) Open a shell window and navigate into the same folder where Router-Buffering.cpp was placed
4) Run the following command (after the colon) in the shell window without the quotations to run the simulation: "g++
   Router-Buffering.cpp -o Router-Buffering; ./Router-Buffering"
5) Perform testing as desired (refer to the Testing Performed section)

### Testing Environment

- UCalgary CPSC Linux Lab (using SSH)

### Testing Performed

- Compared the provided implementation against the professor's demo under identical parameters

### Testing Results

- All the parameter variations used in the professor's demo produced similar results as to what my implementation did
  with the same parameters
    - Please note that I did verify with the professor that the output produced by my implementation was within the
      acceptable variance for the following two cases in particular:
        - Case 1
            - BUFFER_SIZE = 1000
            - TRANSMISSION_SPEED_MEGABITS = 6.0
            - TRACE_FILE = "starwars.txt"
        - Case 2
            - BUFFER_SIZE = 100
            - TRANSMISSION_SPEED_MEGABITS = 6.0
            - TRACE_FILE = "starwars.txt"

### Additional Information

- The provided implementation successfully performed all requirements as outlined on the assignment specifications,
  including the bonus
