# The project's name
project(Effekseer C CXX)

# Check cmake version
cmake_minimum_required(VERSION 3.15)

# C++ version
if(APPLE)
    set(CMAKE_CXX_STANDARD 14)
else()	
    set(CMAKE_CXX_STANDARD 17)
endif()

# For Mac
if(APPLE)
	set(CMAKE_OSX_DEPLOYMENT_TARGET 10.10)
endif()

if (MSVC)
    if(USE_MSVC_RUNTIME_LIBRARY_DLL)
        cmake_policy(SET CMP0091 NEW)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    else()
        cmake_policy(SET CMP0091 NEW)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()
endif()

option(USE_INTERNAL_LOADER "Use Internal Loader" ON)

option(USE_OPENGLES2 "Use OpenGL ES2" OFF)
option(USE_OPENGLES3 "Use OpenGL ES3" OFF)
option(USE_OPENGL3 "Use OpenGL 3" OFF)

if (MSVC)
option(USE_OPENAL "Use OpenAL" OFF)
else()
option(USE_OPENAL "Use OpenAL" ON)
endif()

if (WIN32)
option(USE_XAUDIO2 "Use XAudio2" ON)
option(USE_DSOUND "Use DirectSound" ON)
option(USE_OSM "Use OpenSoundMixer" OFF)
endif()

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

if (USE_INTERNAL_LOADER)
	add_definitions(-D__EFFEKSEER_RENDERER_INTERNAL_LOADER__)
endif()

if (USE_OPENGLES2)
	add_definitions(-D__EFFEKSEER_RENDERER_GLES2__)
endif()

if (USE_OPENGLES3)
	add_definitions(-D__EFFEKSEER_RENDERER_GLES3__)
endif()

if (USE_OPENGL3)
	add_definitions(-D__EFFEKSEER_RENDERER_GL3__)
endif()

add_subdirectory(Effekseer)

if (WIN32)
    add_subdirectory(EffekseerRendererDX9)
    add_subdirectory(EffekseerRendererDX11)
endif()

add_subdirectory(EffekseerRendererGL)

if(BUILD_UNITYPLUGIN)
    add_subdirectory(EffekseerRendererCommon)
endif()

if (BUILD_VIEWER OR BUILD_EDITOR)
    add_subdirectory(3rdParty)
endif()

# set include directories
include_directories(
	Effekseer/
	include/
	)

# set lib directories
link_directories(
	${PROJECT_SOURCE_DIR}/lib/
	)

# Add compile targets
if (BUILD_DX12 OR BUILD_METAL OR BUILD_VULKAN)
    set(BUILD_TEST_TEMP ${BUILD_TEST})
    set(BUILD_TEST OFF)
    set(USE_CREATE_COMPILER_FUNCTION OFF CACHE BOOL "For LLGI" FORCE)
    set(USE_THIRDPARTY_DIRECTORY OFF CACHE BOOL "For LLGI" FORCE)

    if(BUILD_VULKAN)
        set(GLSLANG_INCLUDE_DIR "${GLSLANG_INCLUDE_DIR}" CACHE STRING "For LLGI" FORCE)
    endif()

    add_subdirectory(3rdParty/LLGI)
    set(BUILD_TEST ${BUILD_TEST_TEMP})
endif()

if (BUILD_DX12)
    add_subdirectory(EffekseerRendererDX12)
endif()

if (BUILD_METAL)
    add_subdirectory(EffekseerRendererMetal)
endif()

if (BUILD_VULKAN)
    add_subdirectory(EffekseerRendererVulkan)
endif()

if (USE_OPENAL)
	add_subdirectory(EffekseerSoundAL)
endif()

if (USE_DSOUND)
	add_subdirectory(EffekseerSoundDSound)
endif()

if (USE_XAUDIO2)
	add_subdirectory(EffekseerSoundXAudio2)
endif()

if (USE_OSM OR BUILD_VIEWER)
    add_subdirectory("EffekseerSoundOSMixer")
endif()

if(BUILD_VIEWER OR BUILD_TEST)
    add_subdirectory(3rdParty/spdlog EXCLUDE_FROM_ALL)
    list(APPEND EFK_THIRDPARTY_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/spdlog/include/)
endif()

if (BUILD_VIEWER)
    add_subdirectory("EditorCommon")
    add_subdirectory("Viewer")
    add_subdirectory("IPC")
    add_subdirectory("EffekseerMaterialCompiler")
    add_dependencies(Viewer ResourceData)
endif()


if (BUILD_TEST)
	add_subdirectory("Test")
endif()

if(BUILD_VIEWER)

    set(BUILD_TEST_TEMP ${BUILD_TEST})
    set(BUILD_TEST OFF)
	add_subdirectory("3rdParty/AltseedRHI")
    set(BUILD_TEST ${BUILD_TEST_TEMP})

    if(APPLE)
        add_dependencies(AltseedRHI ExternalProject_zlib ExternalProject_libpng) 
    else()
        add_dependencies(AltseedRHI ExternalProject_glew)
    endif()
    
    target_include_directories(
        AltseedRHI
        PRIVATE
        ${EFK_THIRDPARTY_INCLUDES}
    )

    add_subdirectory("EffekseerMaterial")
    add_subdirectory("EffekseerMaterialEditor")
endif()
