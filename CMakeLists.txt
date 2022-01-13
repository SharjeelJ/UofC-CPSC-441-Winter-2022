# Set minimum cmake version
cmake_minimum_required(VERSION 3.17)

# Set project name
project(UofC_CPSC_441_Winter_2022)

# Set C++ build flags
set(CMAKE_CXX_FLAGS "-Wall")

# Set verbose build
set(CMAKE_VERBOSE_MAKEFILE ON)

# Links the library that will be used
#link_libraries(-lsocket) # Assignment 0

# Add program executable tied to the source file
add_executable(A0_Palindrome-Testing Assignment0/Palindrome-Testing.cpp)
add_executable(A1_Clown-Proxy Assignment1/Clown-Proxy.cpp)
add_executable(A2_Vowelizer Assignment2/Vowelizer.cpp)
add_executable(A3_Router-Buffering Assignment3/Router-Buffering.cpp)
add_executable(A4_Jumping-Bozons Assignment4/Jumping-Bozons.cpp)
