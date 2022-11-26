cmake_minimum_required(VERSION 3.10)

project(TakeScreenshots)

set(effekseer_take_screenshots_src
    main.cpp
    TakeScreenshots.h
    TakeScreenshots.cpp
    Helper.h
    Helper.cpp
)

include_directories(
    ${EFK_THIRDPARTY_INCLUDES}
    ../Effekseer/
    ../EffekseerRendererGL/
    ../EffekseerSoundAL/
    )

if(WIN32)
include_directories(
    ${EFK_THIRDPARTY_INCLUDES}
    ../EffekseerRendererDX11/
    )
endif()

set(common_lib)
set(common_inc)

list(APPEND common_lib EffekseerRendererGL)

if(WIN32)
    list(APPEND common_lib EffekseerRendererDX11)
endif()

list(APPEND common_lib Effekseer)

if (MSVC)
else()
    find_package(Threads REQUIRED)
    list(APPEND common_lib ${CMAKE_THREAD_LIBS_INIT})

    find_package(OpenGL)
    list(APPEND common_inc ${OPENGL_INCLUDE_DIR})
    list(APPEND common_lib ${OPENGL_LIBRARY})

    find_package(OpenAL)
    list(APPEND common_inc ${OPENAL_INCLUDE_DIR})
    list(APPEND common_lib ${OPENAL_LIBRARY})

    if(APPLE)
        find_library(COCOA_FRAMEWORK Cocoa)
        find_library(METAL_LIBRARY Metal)
        find_library(APPKIT_LIBRARY AppKit)
        find_library(METALKIT_LIBRARY MetalKit)
        find_library(QUARTZ_CORE_LIBRARY QuartzCore)
        find_library(IOKIT_FRAMEWORK IOKit)
        find_library(CORE_FOUNDATION_FRAMEWORK CoreFoundation)
        find_library(CORE_VIDEO_FRAMEWORK CoreVideo)
        list(APPEND common_lib
            ${COCOA_FRAMEWORK}
            ${APPKIT_LIBRARY} 
            ${METAL_LIBRARY} 
            ${METALKIT_LIBRARY} 
            ${QUARTZ_CORE_LIBRARY}
            ${OPENGL_LIBRARIES}
            ${IOKIT_FRAMEWORK}
            ${CORE_FOUNDATION_FRAMEWORK}
            ${CORE_VIDEO_FRAMEWORK})
    endif()
endif()

add_executable(${PROJECT_NAME} ${effekseer_take_screenshots_src})
target_link_directories(${PROJECT_NAME} PRIVATE ${EFK_THIRDPARTY_LIBRARY_DIRECTORIES})
target_link_libraries(${PROJECT_NAME} PRIVATE TestRuntimeFramework)

if (NOT APPLE)
    if(WIN32)
    target_link_libraries(
        ${PROJECT_NAME}
        PRIVATE
        debug libglew32d
        optimized libglew32
    )
    else()
    target_link_libraries(
        ${PROJECT_NAME}
        PRIVATE
        GLEW
    )
    endif()
endif()

add_dependencies(TakeScreenshots ExternalProject_glfw)

if(BUILD_WITH_EASY_PROFILER)
    add_dependencies(TakeScreenshots ExternalProject_easy_profiler)
endif()

if(UNIX AND NOT APPLE)
    find_package(X11 REQUIRED)
    target_link_libraries(TakeScreenshots PRIVATE ${X11_LIBRARIES})
    find_package(Threads REQUIRED)
    target_link_libraries(TakeScreenshots PRIVATE ${CMAKE_THREAD_LIBS_INIT})
endif()

if(FROM_CI)
    target_compile_definitions(TakeScreenshots PRIVATE __FROM_CI__)
endif()

# FIXME: Mixed measures with sln. for VS2015
add_definitions(-D__EFFEKSEER_TEST_BUILD_AS_CMAKE__)

if (APPLE)
    add_definitions(-D__EFFEKSEER_RENDERER_GL3__)
endif()

if (MSVC)
    set_target_properties(TakeScreenshots PROPERTIES OUTPUT_NAME_DEBUG "TakeScreenshots")
    set_target_properties(TakeScreenshots PROPERTIES OUTPUT_NAME_RELEASE "TakeScreenshots")
else()
    set_target_properties(TakeScreenshots PROPERTIES OUTPUT_NAME "TakeScreenshots")
endif()

FilterFolder("${effekseer_take_screenshots_src}")

if(CLANG_FORMAT_ENABLED)
    clang_format(TakeScreenshots)
endif()
