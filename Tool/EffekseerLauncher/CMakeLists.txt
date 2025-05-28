cmake_minimum_required(VERSION 3.15)

if(APPLE)
  set(CMAKE_OSX_DEPLOYMENT_TARGET "10.14" CACHE STRING "Minimum OS X deployment version" FORCE)
endif()

project(EffekseerLauncher C CXX)

set(CMAKE_CXX_STANDARD 14)

if(APPLE)
  add_executable(EffekseerLauncher src/main.cpp)
elseif(WIN32)
  cmake_policy(SET CMP0091 NEW)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /SUBSYSTEM:WINDOWS")
  set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /SUBSYSTEM:WINDOWS")

  add_executable(EffekseerLauncher src/main.cpp resources.rc)
elseif(UNIX)
  add_executable(EffekseerLauncher src/main.cpp)
endif()