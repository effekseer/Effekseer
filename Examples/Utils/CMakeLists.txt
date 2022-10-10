cmake_minimum_required (VERSION 3.0.0)

project(Example_Utils)

set(srcs Window.h Window.cpp Input.h Input.cpp)

if(WIN32)
    list(APPEND srcs Windows/Window.h Windows/Window.cpp)
elseif(APPLE)
    list(APPEND srcs Mac/Window.h Mac/Window.mm)
else()
    list(APPEND srcs Linux/Window.h Linux/Window.cpp)
endif()

add_library(Example_Utils STATIC ${srcs})

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "Examples") 
