# Base linux image
FROM ubuntu:22.04 AS build

# Install dependencies
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        build-essential lsb-release wget git ca-certificates python3 python3-pip && \
    rm -rf /var/lib/apt/lists/*

# Install cmake 3.30.0 >= REQUIRED
RUN mkdir -p /opt/cmake-3.30.0
RUN wget https://github.com/Kitware/CMake/releases/download/v3.30.0/cmake-3.30.0-linux-x86_64.sh -O /tmp/cmake-install.sh && \
    chmod +x /tmp/cmake-install.sh && \
    /tmp/cmake-install.sh --skip-license --prefix=/opt/cmake-3.30.0 && \
    ln -s /opt/cmake-3.30.0/bin/cmake /usr/local/bin/cmake && \
    rm /tmp/cmake-install.sh

# Clone rabbitmq-c and its codegen submodule
RUN git clone https://github.com/alanxz/rabbitmq-c.git /opt/rabbitmq-c && \
    cd /opt/rabbitmq-c && \
    git submodule update --init

# Build and install rabbitmq-c
RUN mkdir /opt/rabbitmq-c/build && \
    cd /opt/rabbitmq-c/build && \
    cmake .. -DCMAKE_INSTALL_PREFIX=/usr && \
    make && \
    make install

# Install or-tools src and build from src


# Install or-tools for linux only amd64 supported
RUN wget https://github.com/google/or-tools/releases/download/v9.12/or-tools_amd64_ubuntu-22.04_cpp_v9.12.4544.tar.gz && \
    tar -xzf or-tools_amd64_ubuntu-22.04_cpp_v9.12.4544.tar.gz && \
    cp or-tools_amd64_ubuntu-22.04_cpp_v9.12.4544.tar.gz or-tools && \
    rm or-tools_amd64_ubuntu-22.04_cpp_v9.12.4544.tar.gz

WORKDIR /sCPSolver
COPY src ./src
COPY CMakeLists.txt .

# Compile sCPSolver
RUN mkdir build && \
    cd build && \
    cmake -DORTOOLS_ROOT=/opt/or-tools -DCMAKE_PREFIX_PATH=/opt/or-tools .. && \
    cmake --build .

# Deployable image 
FROM ubuntu:22.04

WORKDIR /sCPSolver

COPY --from=build /sCPSolver/build/scpsolver .
COPY --from=build /opt/or-tools-install /opt/or-tools-install

CMD ["./scpsolver"]