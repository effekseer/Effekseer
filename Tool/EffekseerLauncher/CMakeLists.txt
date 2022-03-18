cmake_minimum_required(VERSION 3.15)

if(APPLE)
  set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "Minimum OS X deployment version" FORCE)
endif()

project(EffekseerLauncher C CXX)

set(CMAKE_CXX_STANDARD 14)

add_executable(EffekseerLauncher src/main.cpp)
