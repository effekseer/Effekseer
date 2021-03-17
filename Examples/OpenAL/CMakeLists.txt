cmake_minimum_required (VERSION 3.0.0)

find_package(OpenAL)

if(OpenAL_FOUND)
    message("Example_OpenAL : OpenAL is found.")

    add_executable(Example_OpenAL main.cpp)

    target_include_directories(
        Example_OpenAL
        PRIVATE
        ${EFK_RUNTIME_ROOT}/Effekseer
        ${EFK_RUNTIME_ROOT}/EffekseerRendererGL
        ${EFK_RUNTIME_ROOT}/EffekseerSoundAL
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
        ${EFK_EXAMPLES_GLFW}
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
            ${OPENGL_LIBRARIES}
            debug "${LIBPNG_LIB_DEBUG}"
            optimized "${LIBPNG_LIB_RELEASE}"
            debug "${ZLIB_LIB_DEBUG}"
            optimized "${ZLIB_LIB_RELEASE}"
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

    if(UNIX AND NOT APPLE)
        find_package(X11 REQUIRED)

        include_directories(${X11_INCLUDE_DIR})

        target_link_libraries(
            Example_OpenAL
            PRIVATE
            Effekseer
            EffekseerRendererGL
            dl
            ${X11_LIBRARIES}
        )
    endif()

else()
    message("Example_OpenAL : OpenAL is not found.")
endif()
