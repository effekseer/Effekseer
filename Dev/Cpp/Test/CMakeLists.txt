
cmake_minimum_required(VERSION 3.10)

set(effekseer_test_src
    graphics_gl.cpp
    main.cpp
    TestHelper.h
    TestHelper.cpp
    Runtime/EffectPlatform.h
    Runtime/EffectPlatform.cpp
    Runtime/EffectPlatformGL.h
    Runtime/EffectPlatformGL.cpp
    Runtime/RuntimeTest.h
    Runtime/RuntimeTest.cpp
)

if(WIN32)
    list(APPEND effekseer_test_src
        Runtime/EffectPlatformDX9.h
        Runtime/EffectPlatformDX9.cpp
        Runtime/EffectPlatformDX11.h
        Runtime/EffectPlatformDX11.cpp
    )
endif()

include_directories(
    ${EFK_THIRDPARTY_INCLUDES}
    ../Effekseer/
    ../EffekseerRendererGL/
    ../EffekseerSoundAL/
    )

link_directories(${PROJECT_SOURCE_DIR}/../lib/)

set(common_lib)
set(common_inc)

list(APPEND common_lib effekseer )
list(APPEND common_lib effekseer_renderer_gl )

if(WIN32)
    list(APPEND common_lib effekseer_renderer_dx11)
    list(APPEND common_lib effekseer_renderer_dx9)
endif()

if (USE_XAUDIO2)
    list(APPEND effekseer_test_src sound_xaudio2.cpp)
    list(APPEND common_lib EffekseerSoundXAudio2)
endif()

if (USE_OPENAL)
    list(APPEND effekseer_test_src sound_al.cpp)
    list(APPEND common_lib EffekseerSoundAL)
endif()

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

     if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        # used in GLFW
        find_library(COCOA_FRAMEWORK Cocoa)
        find_library(COCOA_FRAMEWORK Cocoa)
        find_library(IOKIT_FRAMEWORK IOKit)
        find_library(CORE_FOUNDATION_FRAMEWORK CoreFoundation)
        find_library(CORE_VIDEO_FRAMEWORK CoreVideo)
        list(APPEND common_lib
            ${COCOA_FRAMEWORK}
            ${OPENGL_gl_LIBRARY}
            ${OPENGL_glu_LIBRARY}
            ${IOKIT_FRAMEWORK}
            ${CORE_FOUNDATION_FRAMEWORK}
            ${CORE_VIDEO_FRAMEWORK})
    else()
        list(APPEND common_lib GLEW )
        find_package(X11 REQUIRED)
        list(APPEND common_inc ${X11_INCLUDE_DIR})
        list(APPEND common_lib ${X11_LIBRARIES})
    endif()

endif()

add_executable(Test ${effekseer_test_src} )
target_link_directories(Test PRIVATE ${EFK_THIRDPARTY_LIBRARY_DIRECTORIES})
target_link_libraries(
    Test
    ${common_lib}
    debug glfw3d
    debug OpenSoundMixerd
    optimized glfw3
    optimized OpenSoundMixer
)

add_definitions(-D__EFFEKSEER_RENDERER_INTERNAL_LOADER__)

# FIXME: Mixed measures with sln. for VS2015
add_definitions(-D__EFFEKSEER_TEST_BUILD_AS_CMAKE__)

if (APPLE)
    add_definitions(-D__EFFEKSEER_RENDERER_GL3__)
endif()

if (MSVC)
    set_target_properties(Test PROPERTIES OUTPUT_NAME_DEBUG "Test" )
    set_target_properties(Test PROPERTIES OUTPUT_NAME_RELEASE "Test" )
else()
    set_target_properties(Test PROPERTIES OUTPUT_NAME "Test" )
endif()


foreach(FILE ${effekseer_test_src}) 
    # Get the directory of the source file
    get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)

    # Remove common directory prefix to make the group
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")

    # Make sure we are using windows slashes
    string(REPLACE "/" "\\" GROUP "${GROUP}")

    # Group into "Source Files" and "Header Files"
    if ("${FILE}" MATCHES ".*\\.cpp")
         set(GROUP "Files\\${GROUP}")
    elseif("${FILE}" MATCHES ".*\\.c")
         set(GROUP "Files\\${GROUP}")
    elseif("${FILE}" MATCHES ".*\\.cxx")
         set(GROUP "Files\\${GROUP}")
    elseif("${FILE}" MATCHES ".*\\.h")
         set(GROUP "Files\\${GROUP}")
    elseif("${FILE}" MATCHES ".*\\.mm")
        set(GROUP "Files\\${GROUP}")
    endif()

    source_group("${GROUP}" FILES "${FILE}")
endforeach()