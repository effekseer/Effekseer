cmake_minimum_required(VERSION 3.15)

if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.14" CACHE STRING "Minimum OS X deployment version" FORCE)
endif()

project(Effekseer C CXX)

# C++ version
if(APPLE)
    set(CMAKE_CXX_STANDARD 14)
else()	
    set(CMAKE_CXX_STANDARD 17)
endif()

function(WarnError target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /we4267 /we4456 /we4458 /we4715)
    endif()
endfunction()

if (MSVC)
    if(USE_MSVC_RUNTIME_LIBRARY_DLL)
        cmake_policy(SET CMP0091 NEW)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    else()
        cmake_policy(SET CMP0091 NEW)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()
endif()

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
set_property(TARGET Effekseer PROPERTY FOLDER Libraries)

if(BUILD_UNITYPLUGIN OR BUILD_UNITYPLUGIN_FOR_IOS)
    add_subdirectory(EffekseerRendererCommon)
    set_property(TARGET EffekseerRendererCommon PROPERTY FOLDER Libraries)
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
    set(USE_THIRDPARTY_DIRECTORY ON CACHE BOOL "For LLGI" FORCE)

    if(BUILD_VULKAN)
        set(GLSLANG_INCLUDE_DIR "${GLSLANG_INCLUDE_DIR}" CACHE STRING "For LLGI" FORCE)
    endif()

    add_subdirectory(3rdParty/LLGI)
    set_target_properties (LLGI PROPERTIES FOLDER Thirdparty)
    set(BUILD_TEST ${BUILD_TEST_TEMP})
endif()

if (BUILD_DX9)
    add_subdirectory(EffekseerRendererDX9)
    set_property(TARGET EffekseerRendererDX9 PROPERTY FOLDER Libraries)
endif()

if (BUILD_DX11)
    add_subdirectory(EffekseerRendererDX11)
    set_property(TARGET EffekseerRendererDX11 PROPERTY FOLDER Libraries)
endif()

if (BUILD_GL)
    add_subdirectory(EffekseerRendererGL)
    set_property(TARGET EffekseerRendererGL PROPERTY FOLDER Libraries)
endif()

if (BUILD_DX12)
    add_subdirectory(EffekseerRendererDX12)
    set_property(TARGET EffekseerRendererDX12 PROPERTY FOLDER Libraries)
endif()

if (BUILD_METAL)
    add_subdirectory(EffekseerRendererMetal)
    set_property(TARGET EffekseerRendererMetal PROPERTY FOLDER Libraries)
endif()

if (BUILD_VULKAN)
    add_subdirectory(EffekseerRendererVulkan)
    set_property(TARGET EffekseerRendererVulkan PROPERTY FOLDER Libraries)
endif()

if (USE_OPENAL)
	add_subdirectory(EffekseerSoundAL)
    set_property(TARGET EffekseerSoundAL PROPERTY FOLDER Libraries)
endif()

if (USE_DSOUND)
	add_subdirectory(EffekseerSoundDSound)
    set_property(TARGET EffekseerSoundDSound PROPERTY FOLDER Libraries)
endif()

if (USE_XAUDIO2)
	add_subdirectory(EffekseerSoundXAudio2)
    set_property(TARGET EffekseerSoundXAudio2 PROPERTY FOLDER Libraries)
endif()

if (USE_OSM OR BUILD_VIEWER)
    add_subdirectory("EffekseerSoundOSMixer")
    set_property(TARGET EffekseerSoundOSMixer PROPERTY FOLDER Libraries)
endif()

if(BUILD_VIEWER OR BUILD_TEST)
    add_subdirectory(3rdParty/spdlog EXCLUDE_FROM_ALL)
    list(APPEND EFK_THIRDPARTY_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/spdlog/include/)
endif()

if (BUILD_VIEWER)
    add_subdirectory("EditorCommon")
    set_target_properties (EditorCommon PROPERTIES FOLDER Editor)
    add_subdirectory("Viewer")
    set_target_properties (Viewer PROPERTIES FOLDER Editor)
    set_target_properties (ResourceData PROPERTIES FOLDER Editor)
    add_subdirectory("IPC")
    set_target_properties (IPC PROPERTIES FOLDER Editor)
    add_subdirectory("EffekseerMaterialCompiler")
    add_dependencies(Viewer ResourceData)
endif()


if (BUILD_TEST)
	add_subdirectory("Test")
    set_target_properties (TestCpp PROPERTIES FOLDER Tests)
endif()

if(BUILD_VIEWER)

    set(BUILD_TEST_TEMP ${BUILD_TEST})
    set(BUILD_TEST OFF)
	add_subdirectory("3rdParty/AltseedRHI")
    set_target_properties (AltseedRHI PROPERTIES FOLDER Thirdparty)
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
    set_property(TARGET EffekseerMaterial PROPERTY FOLDER Libraries)
    add_subdirectory("EffekseerMaterialEditor")
    set_property(TARGET EffekseerMaterialEditor PROPERTY FOLDER Editor)
endif()
