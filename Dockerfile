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