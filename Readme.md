# SCPSOLVER

Solve easy, medium, and moderate difficulty constraint programming problems in the scheduling realm effeciently!

## Getting started

This Scheudle solver leverages google OR-Tools and ASIO libraries. Chose not to utilize gRPC, even though it is a fine way to go about this, to increase adoption and ease of use for end developers who want to interact with SCPSOLVE.

### Setup

Note: OR-Tools only provides archives for the x86_64 (also known as amd64) architecture.

### MACOS

### Linux

### Windows
https://developers.google.com/optimization/install/cpp/binary_windows

### Building

#### Unix like
```
mkdir build
cd build
cmake ..
cmake --build .
```

#### Windows
```
cd build
cmake ..
cmake --build .
```