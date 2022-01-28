# Assignment 1: Clown Proxy

### Instructions

1) Download Clown-Proxy.cpp and place it into a new folder
2) Open a shell window and navigate into the same folder where Clown-Proxy.cpp was placed
3) Run the following command (after the colon) in your shell window without the quotations: "g++ Clown-Proxy.cpp -o
   Clown-Proxy; ./Clown-Proxy"
4) Set your browser to use an HTTP proxy with the server address being 127.0.0.1 and port being 9090
5) Perform testing as desired (refer to the URLs Tested section)
6) Press Control + C to terminate the program and make sure to revert changes made in Step 4

### Testing Environment

- macOS 12.2 (21D49)
- Safari Version 15.3 (17612.4.9.1.5)
- 127.0.0.1:9090 was used as a system-wide HTTP proxy
- Requests made using the HTTP/1.1 specification

### URLs Tested

- http://pages.cpsc.ucalgary.ca/~carey/CPSC441/ass1/test1.html
- http://pages.cpsc.ucalgary.ca/~carey/CPSC441/ass1/test2.html
- http://pages.cpsc.ucalgary.ca/~carey/CPSC441/ass1/test3.html
- http://pages.cpsc.ucalgary.ca/~carey/CPSC441/ass1/test4.html
- http://pages.cpsc.ucalgary.ca/~carey/CPSC441/ass1/test5.html
- http://pages.cpsc.ucalgary.ca/~carey/CPSC441/ass1/Floppy.jpg
- http://pages.cpsc.ucalgary.ca/~carey/index.htm
- http://info.cern.ch

### Testing Results

- All tests succeeded
    - On HTML webpages all instances of the word "Happy" were replaced with "Silly" (case-sensitive)
    - On webpages that indicated they contain a jpeg, returned a 302 (temporarily moved) request to one of the two
      following clown images randomly
        - http://pages.cpsc.ucalgary.ca/~carey/CPSC441/ass1/clown1.png
        - http://pages.cpsc.ucalgary.ca/~carey/CPSC441/ass1/clown2.png

### Additional Information

- The provided implementation successfully performed all requirements as outlined on the grading rubric, including the
  bonus
