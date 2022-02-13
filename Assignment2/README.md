# Assignment 2: Vowelizer

### Instructions

1) Download Vowelizer-Server.cpp and Vowelizer-Client.cpp and place both into a new folder
2) Open Vowelizer-Server.cpp in a text editor and set "#define MODE #" as desired (0 = Simple split/merge, 1 = Advanced
   split/merge, 2 = Custom split/merge)
3) Open two shell windows and navigate both into the same folder where Vowelizer-Server.cpp and Vowelizer-Client.cpp
   were placed
4) Run the following command (after the colon) in the first shell window without the quotations to run the server: "g++
   Vowelizer-Server.cpp -o Vowelizer-Server; ./Vowelizer-Server"
5) Run the following command (after the colon) in the second shell window without the quotations to run the client: "g++
   Vowelizer-Client.cpp -o Vowelizer-Client; ./Vowelizer-Client"
6) Perform testing as desired using the client (refer to the Testing Performed section)
7) Press Control + C to terminate the server and/or client (client can also be terminated using the Exit menu selection)

### Testing Environment

- UCalgary CPSC Linux Lab (using SSH)

### Testing Performed

- TODO

### Testing Results

- All tests succeeded
    - TODO

### Additional Information

- The provided implementation successfully performed all requirements as outlined on the assignment specifications,
  including the bonus
- The implementation may handle cases where the user is providing malformed/incorrect input, however when provided valid
  input should perform as expected
- A string containing just an empty space (so " ") is returned instead of a empty/blank message where appropriate,
  leading to a string of 1 byte being returned (as you cannot send empty strings over TCP / UDP)
