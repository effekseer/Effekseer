cmake_minimum_required(VERSION 3.10)

project(OpenGLHelper)

set(srcs
    OpenGLExtensions.h
    OpenGLExtensions.cpp
)

add_library(OpenGLHelper STATIC ${srcs})

target_include_directories(
    OpenGLHelper
    PUBLIC
    ${PROJECT_SOURCE_DIR}
    ${EFK_THIRDPARTY_INCLUDES}
)

if(TARGET ExternalProject_glfw)
    add_dependencies(OpenGLHelper ExternalProject_glfw)
endif()

if(CLANG_FORMAT_ENABLED)
    clang_format(OpenGLHelper)
endif()
