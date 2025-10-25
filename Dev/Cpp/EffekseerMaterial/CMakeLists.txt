# The project's name
project(EffekseerMaterial)

# Check cmake version
cmake_minimum_required(VERSION 3.10)

# For MSVC
if (MSVC)
    option(USE_MSVC_RUNTIME_LIBRARY_DLL "Use MSVC runtime library DLL" OFF)
endif()

# C++ version
set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE hs *.h)

if(APPLE)
    file(GLOB_RECURSE srcs *.mm *.cpp)
else()
    file(GLOB_RECURSE srcs *.cpp)
endif()

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

if(CLANG_FORMAT_ENABLED)
    clang_format(EffekseerMaterial)
endif()

FilterFolder("${files}")
