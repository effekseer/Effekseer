# The project's name
project(EffekseerMaterial)

# Check cmake version
cmake_minimum_required(VERSION 3.0)

# For MSVC
if (MSVC)
    option(USE_MSVC_RUNTIME_LIBRARY_DLL "Use MSVC runtime library DLL" OFF)
endif()

# C++ version
if(WIN32)
    set(CMAKE_CXX_STANDARD 17)
else()
    set(CMAKE_CXX_STANDARD 14)
endif()

file(GLOB_RECURSE hs *.h)

if(APPLE)
    file(GLOB_RECURSE srcs *.mm *.cpp)
else()
    file(GLOB_RECURSE srcs *.cpp)
endif()

# set include directories
include_directories(../include/)

# Add compile targets
add_library( EffekseerMaterial STATIC ${hs} ${srcs} )

# Flags
if (MSVC)
    # CMAKE_MSVC_RUNTIME_LIBRARY can be only used if minimum required is larger than 3.17
    if(USE_MSVC_RUNTIME_LIBRARY_DLL)
        cmake_policy(SET CMP0091 NEW)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    else()
        cmake_policy(SET CMP0091 NEW)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()
endif()

set(files ${srcs} ${hs})

FilterFolder("${files}")
