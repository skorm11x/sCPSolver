# SCPSOLVER

Solve easy, medium, and moderate difficulty constraint programming problems in the scheduling realm efficiently!

This program utilizes rabbitmq-c as its connection layer for Remote Procedure Call and relies upon Google's or-tools for multithreaded CSP-SAT problem solving. 
https://github.com/alanxz/rabbitmq-c
https://developers.google.com/optimization/install/cpp/binary_linux#ubuntu_1

Long running problems could introduce scalability problems that are not currently being addressed (e.g. tasking the program with multiple long running problems) but are theorized to be possibly addressed with cloud auto scaling solutions.

## Getting started

This Schedule solver leverages google OR-Tools and ASIO libraries. Chose not to utilize gRPC, even though it is a fine way to go about this, to increase adoption and ease of use for end developers who want to interact with SCPSOLVE.

### Setup

Note: OR-Tools only provides archives for the x86_64 (also known as amd64) architecture.

### Linux amd64

You need to have or-tools installed on your system under /opt/or-tools:
```
wget https://github.com/google/or-tools/releases/latest/download/or-tools_debian12_x86_64_cpp_v9.14.6206.zip && \
    unzip or-tools_debian12_x86_64_cpp_v9.14.6206.zip -d /opt/or-tools && \
    rm or-tools_debian12_x86_64_cpp_v9.14.6206.zip
```

Replace the endpoint with your appropriate system endpoint.

### Building

Install rabbitmq-c onto machine
```
git clone https://github.com/alanxz/rabbitmq-c.git
cd rabbitmq-c
git submodule update --init
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make
sudo make install
```

#### Linux amd64
```
mkdir build
cd build
cmake ..
cmake --build .
```

### Deploying

A docker image is used to deploy with a single docker compose file also provided.
