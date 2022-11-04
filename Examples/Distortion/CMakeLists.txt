cmake_minimum_required (VERSION 3.0.0)

project(Example_Distortion)

set (DEVICE_SRC_FILES ../OpenGL/DeviceGLFW.cpp ../OpenGL/DeviceGLFW.h)

if (WIN32)
    list (APPEND DEVICE_SRC_FILES ../DirectX9/DeviceDX9.cpp ../DirectX9/DeviceDX9.h)
    list (APPEND DEVICE_SRC_FILES ../DirectX11/DeviceDX11.cpp ../DirectX11/DeviceDX11.h)
endif ()

add_executable(${PROJECT_NAME} main.cpp ${DEVICE_SRC_FILES})

set (MODULE_INCLUDE_PATHS ${EFK_RUNTIME_ROOT}/Effekseer ${EFK_RUNTIME_ROOT}/EffekseerRendererGL ../Utils)

if (WIN32)
    list (APPEND MODULE_INCLUDE_PATHS ${EFK_RUNTIME_ROOT}/EffekseerRendererDX9)
    list (APPEND MODULE_INCLUDE_PATHS ${EFK_RUNTIME_ROOT}/EffekseerRendererDX11)
    list (APPEND MODULE_INCLUDE_PATHS ${EFK_RUNTIME_ROOT}/EffekseerSoundXAudio2)
endif ()

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
    ${MODULE_INCLUDE_PATHS}
	${EFK_THIRDPARTY_INCLUDES}
)

target_link_directories(
    ${PROJECT_NAME}
    PRIVATE
    ${EFK_THIRDPARTY_LIBRARY_DIRECTORIES}
)

set (MODULE_LIBRARIES Effekseer EffekseerRendererGL ${EFK_EXAMPLES_GLFW} Example_Utils)

if (WIN32)
    list (APPEND MODULE_LIBRARIES EffekseerRendererDX9)
    list (APPEND MODULE_LIBRARIES EffekseerRendererDX11)
    list (APPEND MODULE_LIBRARIES EffekseerSoundXAudio2)
endif ()

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    ${MODULE_LIBRARIES}
)

if(WIN32)
    # for Visual C++
    target_compile_definitions(
        ${PROJECT_NAME}
        PRIVATE
        UNICODE
        _UNICODE
    )
endif()

if(NOT WIN32)
    find_package(Threads REQUIRED)
    find_package(OpenGL REQUIRED)
    target_link_libraries(
        ${PROJECT_NAME}
        PRIVATE
        ${CMAKE_THREAD_LIBS_INIT}
        ${OPENGL_LIBRARIES}
    )
endif()

if(APPLE)
    find_library(COCOA_FRAMEWORK Cocoa)
    find_library(IOKIT_FRAMEWORK IOKit)
    find_library(CORE_FOUNDATION_FRAMEWORK CoreFoundation)
    find_library(CORE_VIDEO_FRAMEWORK CoreVideo)

    target_link_libraries(
        ${PROJECT_NAME}
        PRIVATE
        ${COCOA_FRAMEWORK}
        ${IOKIT_FRAMEWORK}
        ${CORE_FOUNDATION_FRAMEWORK}
        ${CORE_VIDEO_FRAMEWORK}
    )
endif()

if(UNIX AND NOT APPLE)
    find_package(X11 REQUIRED)

    include_directories(${X11_INCLUDE_DIR})

    target_link_libraries(
        ${PROJECT_NAME}
        PRIVATE
        Effekseer
        EffekseerRendererGL
        dl
        ${X11_LIBRARIES}
    )
endif()

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "Examples") 

if(USE_LIBPNG_LOADER)
    target_link_directories(
        ${PROJECT_NAME}
        PRIVATE
        ${EFK_THIRDPARTY_LIBRARY_DIRECTORIES}
    )

    target_link_libraries(
        ${PROJECT_NAME}
        PRIVATE
        debug "${LIBPNG_LIB_DEBUG}"
        optimized "${LIBPNG_LIB_RELEASE}"
        debug "${ZLIB_LIB_DEBUG}"
        optimized "${ZLIB_LIB_RELEASE}"
	)
endif()

