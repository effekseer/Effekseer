cmake_minimum_required (VERSION 3.15)

project(Example_NetworkClient)

add_executable(${PROJECT_NAME} main.cpp ../OpenGL/DeviceGLFW.cpp ../OpenGL/DeviceGLFW.h)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
    ${EFK_RUNTIME_ROOT}/Effekseer
    ${EFK_RUNTIME_ROOT}/EffekseerRendererGL
    ../Utils
	${EFK_THIRDPARTY_INCLUDES}
)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    Effekseer
    EffekseerRendererGL
    Example_Utils
    ${EFK_EXAMPLES_GLFW}
)

target_link_directories(
    ${PROJECT_NAME}
    PRIVATE
    ${EFK_THIRDPARTY_LIBRARY_DIRECTORIES}
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

LinkAppleLibs(${PROJECT_NAME})

if(UNIX AND NOT APPLE)
    find_package(X11 REQUIRED)

    target_include_directories(
        ${PROJECT_NAME}
        PRIVATE
        ${X11_INCLUDE_DIR})

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
    target_link_libraries(
        ${PROJECT_NAME}
        PRIVATE
        debug "${LIBPNG_LIB_DEBUG}"
        optimized "${LIBPNG_LIB_RELEASE}"
        debug "${ZLIB_LIB_DEBUG}"
        optimized "${ZLIB_LIB_RELEASE}"
	)
endif()
