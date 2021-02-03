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

target_include_directories(
    IPC
    PRIVATE
    ${EFK_THIRDPARTY_INCLUDES})

if(NOT WIN32 AND NOT APPLE)

    target_link_libraries(
        IPC
        PRIVATE 
        rt)

endif()
