
cmake_minimum_required(VERSION 3.10)

project(Test)

set(effekseer_test_src
    main.cpp
    TestHelper.h
    TestHelper.cpp
    Window/RenderingWindow.h
    Window/RenderingWindow.cpp
    Window/RenderingWindowGL.h
    Window/RenderingWindowGL.cpp
    Runtime/EffectPlatform.h
    Runtime/EffectPlatform.cpp
    Runtime/EffectPlatformGLFW.h
    Runtime/EffectPlatformGLFW.cpp
    Runtime/EffectPlatformGL.h
    Runtime/EffectPlatformGL.cpp
    Runtime/RuntimeTest.cpp
    Runtime/Performance.cpp
    Runtime/TextureFormats.cpp
    Runtime/Vertex.cpp
    Runtime/ResourceManager.cpp
    Backend/Mesh.cpp
    Backend/RenderPassTest.cpp
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

if(BUILD_METAL)
    list(APPEND effekseer_test_src
        Runtime/EffectPlatformMetal.h
        Runtime/EffectPlatformMetal.mm
    )
endif()

if(BUILD_VULKAN)
    list(APPEND effekseer_test_src
        Runtime/EffectPlatformVulkan.h
        Runtime/EffectPlatformVulkan.cpp
    )
endif()

if(WIN32)
    list(APPEND effekseer_test_src
        Runtime/EffectPlatformDX9.h
        Runtime/EffectPlatformDX9.cpp
        Runtime/EffectPlatformDX11.h
        Runtime/EffectPlatformDX11.cpp
        Window/RenderingWindowDX11.h
        Window/RenderingWindowDX11.cpp
    )
endif()

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
    list(APPEND common_lib EffekseerRendererDX9)
endif()

if(BUILD_DX12 OR BUILD_METAL OR BUILD_VULKAN)
    list(APPEND common_lib LLGI)
endif()

if(BUILD_DX12)
    list(APPEND common_lib EffekseerRendererDX12)
endif()


if(BUILD_METAL)
    list(APPEND common_lib EffekseerRendererMetal)
endif()

if(BUILD_VULKAN)
    list(APPEND common_lib EffekseerRendererVulkan)
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

add_executable(Test ${effekseer_test_src} )
target_link_directories(Test PRIVATE ${EFK_THIRDPARTY_LIBRARY_DIRECTORIES})

if(BUILD_VULKAN)
    find_package(Vulkan REQUIRED)

    target_include_directories(
        Test
        PRIVATE
        ${Vulkan_INCLUDE_DIRS}
    )
    
    target_link_libraries(
        Test
        PRIVATE 
        ${Vulkan_LIBRARIES}
    )

    # TODO Improve it
    target_link_libraries(
        Test
        PRIVATE 
        glslang
        SPIRV
        OGLCompiler
        OSDependent
    )
    
    target_link_directories(
        Test
        PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/../3rdParty/LLGI/EP/Install/glslang/lib
    )
endif()

add_dependencies(Test ExternalProject_glfw)

if(BUILD_WITH_EASY_PROFILER)
    add_dependencies(Test ExternalProject_easy_profiler)
endif()

if (BUILD_WITH_POSTFIX)
target_link_libraries(
    Test
    PRIVATE 
    ${common_lib}
    debug glfw3d
    optimized glfw3
)
else()
target_link_libraries(
    Test
    PRIVATE 
    ${common_lib}
    glfw3
    dl
)
endif()

if(UNIX AND NOT APPLE)
    find_package(X11 REQUIRED)
    target_link_libraries(Test PRIVATE ${X11_LIBRARIES})
    find_package(Threads REQUIRED)
    target_link_libraries(Test PRIVATE ${CMAKE_THREAD_LIBS_INIT})
endif()

if(FROM_CI)
    target_compile_definitions(Test PRIVATE __FROM_CI__)
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

if(CLANG_FORMAT_ENABLED)
    clang_format(Test)
endif()

if(USE_LIBPNG_LOADER)
    add_dependencies(Test ExternalProject_libpng)


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

