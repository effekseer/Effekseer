# The project's name
project(IPC)

# Check cmake version
cmake_minimum_required(VERSION 3.0)

add_library(
    IPC
    STATIC
    IPC.h
    IPC.cpp
)