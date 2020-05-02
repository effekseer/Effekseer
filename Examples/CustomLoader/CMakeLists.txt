cmake_minimum_required (VERSION 3.0.0)

project(Example_CustomLoader)

add_executable(Example_CustomLoader main.cpp)

target_include_directories(
    Example_CustomLoader
    PRIVATE
    ${EFK_RUNTIME_ROOT}/Effekseer
    ${EFK_RUNTIME_ROOT}/EffekseerRendererGL
    ../Utils
	${EFK_THIRDPARTY_INCLUDES}
)

target_link_libraries(
    Example_CustomLoader
    PRIVATE
    Effekseer
    EffekseerRendererGL
    Example_Utils
    ${EFK_EXAMPLES_GLFW}
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
        Example_CustomLoader
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
        Example_CustomLoader
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
        Example_CustomLoader
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
