cmake_minimum_required (VERSION 3.0.0)

add_executable(Example_CustomLoader main.cpp)

target_include_directories(
    Example_CustomLoader
    PRIVATE
    ../../Dev/Cpp/Effekseer
    ../../Dev/Cpp/EffekseerRendererGL
    ../Utils
	${EFK_THIRDPARTY_INCLUDES}
)

target_link_libraries(
    Example_CustomLoader
    PRIVATE
    Effekseer
    EffekseerRendererGL
    Example_Utils
    debug "${GLFW3_LIB_DEBUG}"
    optimized "${GLFW3_LIB_RELEASE}"
)

target_link_directories(
    Example_CustomLoader
    PRIVATE
    ${EFK_THIRDPARTY_LIBRARY_DIRECTORIES}
)

if(NOT WIN32)
    find_package(Threads REQUIRED)
    find_package(OpenGL REQUIRED)
    target_link_libraries(
        Example_OpenGL
        PRIVATE
        ${CMAKE_THREAD_LIBS_INIT}
        ${OPENGL_gl_LIBRARY}
        debug "${LIBPNG_LIB_DEBUG}"
        optimized "${LIBPNG_LIB_RELEASE}"
        z
    )
endif()

if(APPLE)
    find_library(COCOA_FRAMEWORK Cocoa)
    find_library(IOKIT_FRAMEWORK IOKit)
    find_library(CORE_FOUNDATION_FRAMEWORK CoreFoundation)
    find_library(CORE_VIDEO_FRAMEWORK CoreVideo)

    target_link_libraries(
        Example_OpenGL
        PRIVATE
        ${COCOA_FRAMEWORK}
        ${IOKIT_FRAMEWORK}
        ${CORE_FOUNDATION_FRAMEWORK}
        ${CORE_VIDEO_FRAMEWORK}
    )
endif()