
cmake_minimum_required(VERSION 3.10)

project(Test)

set(effekseer_test_src
    graphics_gl.cpp
    main.cpp
    TestHelper.h
    TestHelper.cpp
    Runtime/EffectPlatform.h
    Runtime/EffectPlatform.cpp
    Runtime/EffectPlatformGLFW.h
    Runtime/EffectPlatformGLFW.cpp
    Runtime/EffectPlatformGL.h
    Runtime/EffectPlatformGL.cpp
    Runtime/RuntimeTest.h
    Runtime/RuntimeTest.cpp
)

if(BUILD_DX12 OR BUILD_METAL OR BUILD_VULKAN)
    list(APPEND effekseer_test_src
        Runtime/EffectPlatformLLGI.h
        Runtime/EffectPlatformLLGI.cpp
    )
endif()

if(BUILD_DX12)
    list(APPEND effekseer_test_src
        Runtime/EffectPlatformDX12.h
        Runtime/EffectPlatformDX12.cpp
    )
endif()

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

set(common_lib)
set(common_inc)

list(APPEND common_lib Effekseer)
list(APPEND common_lib EffekseerRendererGL)

if(WIN32)
    list(APPEND common_lib EffekseerRendererDX11)
    list(APPEND common_lib EffekseerRendererDX9)
endif()

if (USE_XAUDIO2)
    list(APPEND effekseer_test_src sound_xaudio2.cpp)
    list(APPEND common_lib EffekseerSoundXAudio2)
endif()

if (USE_OPENAL)
    list(APPEND effekseer_test_src sound_al.cpp)
    list(APPEND common_lib EffekseerSoundAL)
endif()

if(BUILD_DX12 OR BUILD_METAL OR BUILD_VULKAN)
    list(APPEND common_lib LLGI)
endif()

if(BUILD_DX12)
    list(APPEND common_lib EffekseerRendererDX12)
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

if (MSVC)
target_link_libraries(
    Test
    ${common_lib}
    debug glfw3d
    debug OpenSoundMixerd
    optimized glfw3
    optimized OpenSoundMixer
)
else()
target_link_libraries(
    Test
    ${common_lib}
    glfw3
    OpenSoundMixer
)
endif()

add_definitions(-D__EFFEKSEER_RENDERER_INTERNAL_LOADER__)

# FIXME: Mixed measures with sln. for VS2015
add_definitions(-D__EFFEKSEER_TEST_BUILD_AS_CMAKE__)

if (APPLE)
    add_definitions(-D__EFFEKSEER_RENDERER_GL3__)
endif()

if (MSVC)
    set_target_properties(Test PROPERTIES OUTPUT_NAME_DEBUG "Test")
    set_target_properties(Test PROPERTIES OUTPUT_NAME_RELEASE "Test")
else()
    set_target_properties(Test PROPERTIES OUTPUT_NAME "Test")
endif()

FilterFolder("${effekseer_test_src}")

if(USE_LIBPNG_LOADER)
    target_link_directories(
        ${PROJECT_NAME}
        PRIVATE
        ${EFK_THIRDPARTY_LIBRARY_DIRECTORIES}
    )

    target_link_libraries(
        ${PROJECT_NAME}
        debug "${ZLIB_LIB_DEBUG}"
        optimized "${ZLIB_LIB_RELEASE}"
        debug "${LIBPNG_LIB_DEBUG}"
	    optimized "${LIBPNG_LIB_RELEASE}"
	)
endif()

