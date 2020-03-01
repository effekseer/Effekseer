cmake_minimum_required (VERSION 3.0.0)

find_package(OpenAL)

if(OpenAL_FOUND)
    message("Example_OpenAL : OpenAL is found.")

    add_executable(Example_OpenAL main.cpp)

    target_include_directories(
        Example_OpenAL
        PRIVATE
        ../../Dev/Cpp/Effekseer
        ../../Dev/Cpp/EffekseerRendererGL
        ../../Dev/Cpp/EffekseerSoundAL
        ../Utils
        ${EFK_THIRDPARTY_INCLUDES}
        ${OPENAL_INCLUDE_DIR}
    )

    target_link_libraries(
        Example_OpenAL
        PRIVATE
        Effekseer
        EffekseerRendererGL
        EffekseerSoundAL
        Example_Utils
        debug "${GLFW3_LIB_DEBUG}"
        optimized "${GLFW3_LIB_RELEASE}"
        ${OPENAL_LIBRARY}
    )

    target_link_directories(
        Example_OpenAL
        PRIVATE
        ${EFK_THIRDPARTY_LIBRARY_DIRECTORIES}
    )

    if(NOT WIN32)
        find_package(Threads REQUIRED)
        find_package(OpenGL REQUIRED)
        target_link_libraries(
            Example_OpenAL
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
            Example_OpenAL
            PRIVATE
            ${COCOA_FRAMEWORK}
            ${IOKIT_FRAMEWORK}
            ${CORE_FOUNDATION_FRAMEWORK}
            ${CORE_VIDEO_FRAMEWORK}
        )
    endif()
else()
    message("Example_OpenAL : OpenAL is not found.")
endif()