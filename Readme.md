# SCPSOLVER

Solve easy, medium, and moderate difficulty constraint programming problems in the scheduling realm efficiently!

This program utilizes Civet server as its Rest callback based execution for RPC and upon Google's or-tools for multithreaded CSP-SAT problem solving. 
https://developers.google.com/optimization/install/cpp/binary_linux#ubuntu_1

Long running problems could introduce scalability problems that are not currently being addressed (e.g. tasking the program with multiple long running problems) but are theorized to be possibly addressed with cloud auto scaling solutions.

## Getting started

This Schedule solver leverages google OR-Tools and ASIO libraries. Chose not to utilize gRPC, even though it is a fine way to go about this, to increase adoption and ease of use for end developers who want to interact with SCPSOLVE.

### Setup && Prereqs

#### Google or-tools
Note: OR-Tools only provides archives for the x86_64 (also known as amd64) architecture.

Follow the official google guides for installing and setting up or-tools:
https://developers.google.com/optimization/install/cpp

### Building Linux && MACOS

#### MACOS
Install or-tools onto machine via cloning the repo and building as a local lib
```
wget https://github.com/google/or-tools/releases/download/v9.14/or-tools_arm64_macOS-15.5_cpp_v9.14.6206.tar.gz
tar -xzf or-tools_arm64_macOS-15.5_cpp_v9.14.6206.tar.gz
rm or-tools_arm64_macOS-15.5_cpp_v9.14.6206.tar.gz
mkdir lib/or-tools
mv or-tools_arm64_macOS-15.5_cpp_v9.14.6206 lib/or-tools
```

Install civetweb onto machine via cloning the repo and building as a local lib
```
git clone https://github.com/civetweb/civetweb.git
cd civetweb
cmake -S . -B build && cmake --build build
```

### Deploying

A docker image is used to deploy with a single docker compose file also provided.
You can also compile and run locally

#### Local

```
cmake -S . -B build
cmake --build build
cd build
```

sign it locally to assist with mac developer settings:
```
codesign --force --sign - scpsolver
codesign --force --sign - lib/or-tools/lib/*.dylib
```

or alternatively (temporarily) allow code exeution from unsigned srcs
```
sudo spctl --master-disable
```

re-enable when done
```
sudo spctl --master-disable
```
I recommend you use your Apple developer ID and just sign your binary and all the libs.

Run
```
./scpsolver
```

#### Dockerfile
WIP, update linux build and mac builds. Docker base needs to detect host ISA to pull right binaries

### Examples

#### Sudoku
take from https://sandiway.arizona.edu/sudoku/examples.html
O*(d^n) where d = domain size max, n = number variables

basic sudoku tester

## TODOS
1. Start using the Google test suite instead of random curl bash scripts 
2. 

### DOXYOGEN setup

Great resource
https://darkognu.eu/programming/tutorials/doxygen_tutorial_cpp/#installing-the-required-software

```
brew install doxygen
brew install graphviz
```