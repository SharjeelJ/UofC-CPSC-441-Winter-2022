# Assignment 2: Vowelizer

### Instructions

1) Download Vowelizer-Server.cpp and Vowelizer-Client.cpp and place both into a new folder
2) Open Vowelizer-Server.cpp in a text editor and set the int value of "#define MODE" as desired (0 = Simple
   split/merge, 1 = Advanced split/merge, 2 = Custom split/merge)
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

### Testing Performed (Strings Contained Inside "")

- Simple Split (MODE 0)
    - `"Hello there!"`
    - `""`
    - `" "`
    - `"sdf"`
    - `"aei"`
    - `" He,L  l.O _ tHEeRe?! "`
- Simple Merge (MODE 0)
    - `"H ll  th r !" & " e  o   e e "`
    - `""` & `""`
    - `" "` & `" "`
    - `"sdf"` & `"   "`
    - `"   "` & `"aei"`
    - `" H ,L  l.  _ tH  R ?! "` & `"  e      O     Ee e   "`
- Advanced Split (MODE 1)
    - `"Hello there!"`
    - `""`
    - `" "`
    - `"sdf"`
    - `"aei"`
    - `" He,L  l.O _ tHEeRe?! "`
- Advanced Merge (MODE 1)
    - `"Hll thr!" & "1e2o3e1e"`
    - `""` & `""`
    - `" "` & `" "`
    - `"sdf"` & `""`
    - `""` & `"0a0e0i"`
    - `" H,L  l. _ tHR?! "` & `"2e6O5E0e1e"`
- Custom Split (MODE 2)
    - TODO
- Custom Merge (MODE 2)
    - TODO

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
