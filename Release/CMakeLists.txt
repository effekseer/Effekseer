cmake_minimum_required(VERSION 3.15)
project(Effekseer)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(FilterFolder)

option(BUILD_EXAMPLES "Build examples" ON)

option(BUILD_GL "Build OpenGL" ON)
option(BUILD_VULKAN "Build vulkan modules" OFF)

if (WIN32)
    option(BUILD_DX9 "Build DirectX9 modules" ON)
    option(BUILD_DX11 "Build DirectX11 modules" ON)
    option(BUILD_DX12 "Build DirectX12 modules" ON)
else()
    option(BUILD_DX9 "Build DirectX9 modules" OFF)
    option(BUILD_DX11 "Build DirectX11 modules" OFF)
    option(BUILD_DX12 "Build DirectX12 modules" OFF)
endif()

if (APPLE)
    option(BUILD_METAL "Build metal modules" ON)
else()
    option(BUILD_METAL "Build metal modules" OFF)
endif()

# For MSVC
if (MSVC)
    option(USE_MSVC_RUNTIME_LIBRARY_DLL "Use MSVC runtime library DLL" OFF)
endif()

# MSVC Flag

if (MSVC)
    if(USE_MSVC_RUNTIME_LIBRARY_DLL)
        cmake_policy(SET CMP0091 NEW)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    else()
        cmake_policy(SET CMP0091 NEW)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()
endif()


if(BUILD_VULKAN)
    option(USE_EXTERNAL_GLSLANG "Whether do it compile with external glslang" OFF)

    if(USE_EXTERNAL_GLSLANG)
        option(GLSLANG_INCLUDE_DIR "A directory which contains glslang includes" "")
    else()
        set(ENABLE_HLSL OFF CACHE BOOL "For glslang" FORCE)
        set(ENABLE_CTEST OFF CACHE BOOL "For glslang" FORCE)
        set(ENABLE_GLSLANG_BINARIES OFF CACHE BOOL "For glslang" FORCE)

        set(GLSLANG_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/3rdParty/glslang/" "${CMAKE_CURRENT_SOURCE_DIR}/src/3rdParty/")
        add_subdirectory(src/3rdParty/glslang/)
    endif()
endif()

set(EFK_RUNTIME_ROOT ${CMAKE_CURRENT_LIST_DIR}/src)

add_subdirectory(src)

if (BUILD_EXAMPLES)
    add_subdirectory(Examples/Utils/glfw)
    add_subdirectory(Examples)
endif()
