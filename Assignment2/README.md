# Assignment 2: Vowelizer

### Instructions

1) Download Vowelizer-Server.cpp and Vowelizer-Client.cpp and place both into a new folder
2) Open Vowelizer-Server.cpp in a text editor and set the int value of "#define MODE" (line 19) as desired (0 = Simple
   split/merge, 1 = Advanced split/merge, 2 = Custom split/merge)
3) Open Vowelizer-Client.cpp in a text editor and set the string value of "#define IP" (line 18) as desired (string
   should be of the format "X.X.X.X" which is the IPv4 address of the server)
4) Open two shell windows and navigate both into the same folder where Vowelizer-Server.cpp and Vowelizer-Client.cpp
   were placed
5) Run the following command (after the colon) in the first shell window without the quotations to run the server: "g++
   Vowelizer-Server.cpp -o Vowelizer-Server; ./Vowelizer-Server"
6) Run the following command (after the colon) in the second shell window without the quotations to run the client: "g++
   Vowelizer-Client.cpp -o Vowelizer-Client; ./Vowelizer-Client"
7) Perform testing as desired using the client (refer to the Testing Performed section)
8) Press Control + C to terminate the server and/or client (client can also be terminated using the Exit menu selection)

### Testing Environment

- Server running on UCalgary CPSC Linux Compute CSX3 (using SSH)
- Client running on UCalgary CPSC Linux Lab (using SSH)

### Testing Performed (Strings Contained Inside "")

- All three of the modes (simple, advanced and custom) had their split and merge functionality tested on the following
  strings
    - `"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"`
    - `"Grocery list: milk, bread, and eggs."`
    - `"The quick brown fox jumped over the lazy dog."`
    - `"The Calgary Flames fortuitously won their game 6 to 5 in overtime last week. Cool!"`
    - `"The postal code for the University of Calgary is T2N 1N4."`
    - `"In the teen song "Jenny I Need Your Number", her number is 7-5309 if I recall correctly."`
    - `"It was the best of times; it was the worst of times."`
    - `"My favourite winter Olympic sports are curling, skiing, and bobsledding."`
    - `"Facetiously, I said "Curling rocks!"."`
    - `"Roses are red, Violets are blue, If you can do this test case, You'll score more than 2."`
    - `"When cleaning the garage, I screamed "Eeeeeeeeek! I see a mouse!!!""`
    - `"I wonder if there is a DNS entry for www.goooooooogle.com?"`

### Testing Results

- All tests succeeded
    - Tests that return an empty/blank string (so "" with a size of 0 byte) will instead return a string with a blank
      space (so " " with a size of 1 byte)

### Additional Information

- The provided implementation successfully performed all requirements as outlined on the assignment specifications,
  including the bonus
- The provided implementation may handle cases where the user is providing malformed/incorrect input, however when
  provided valid input should perform as expected
- A string containing just an empty space (so " ") is returned instead of an empty/blank message where appropriate,
  leading to a string of 1 byte being returned (as you cannot send empty strings over TCP/UDP)
