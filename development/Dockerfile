# This is the base image used for development and testing
# I use CLion to run the source code inside this container
FROM ubuntu:22.04

RUN apt-get update \
    && apt-get install -y build-essential \
    gcc \
    g++ \
    cmake \
    gdb \
    cppcheck \
    clang-tidy \
    && apt-get clean