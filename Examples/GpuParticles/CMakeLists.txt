cmake_minimum_required (VERSION 3.15)

project(Example_GpuParticles)

if (BUILD_VULKAN)
    find_package(Vulkan REQUIRED)
endif ()

set (DEVICE_SRC_FILES ../OpenGL/DeviceGLFW.cpp ../OpenGL/DeviceGLFW.h)

if (WIN32)
    list (APPEND DEVICE_SRC_FILES ../DirectX11/DeviceDX11.cpp ../DirectX11/DeviceDX11.h)
    list (APPEND DEVICE_SRC_FILES ../DirectX12/DeviceDX12.cpp ../DirectX12/DeviceDX12.h)
endif ()

if (BUILD_VULKAN)
    list (APPEND DEVICE_SRC_FILES ../Vulkan/DeviceVulkan.cpp ../Vulkan/DeviceVulkan.h)
endif ()

if (BUILD_METAL)
    list (APPEND DEVICE_SRC_FILES ../Metal/DeviceMetal.mm ../Metal/DeviceMetal.h)
endif ()

if (APPLE)
    add_executable(${PROJECT_NAME} main.mm ${DEVICE_SRC_FILES})
else ()
    add_executable(${PROJECT_NAME} main.cpp ${DEVICE_SRC_FILES})
endif ()

set (MODULE_INCLUDE_PATHS
    ${EFK_RUNTIME_ROOT}/Effekseer
    ${EFK_RUNTIME_ROOT}/EffekseerRendererGL
    ${EFK_RUNTIME_ROOT}/EffekseerRendererLLGI
    ${EFK_RUNTIME_ROOT}/3rdParty/LLGI/src/
    ../Utils
)

if (WIN32)
    list (APPEND MODULE_INCLUDE_PATHS
        ${EFK_RUNTIME_ROOT}/EffekseerRendererDX11
        ${EFK_RUNTIME_ROOT}/EffekseerRendererDX12
        ${EFK_RUNTIME_ROOT}/EffekseerSoundXAudio2
    )
endif ()

if (BUILD_VULKAN)
    list (APPEND MODULE_INCLUDE_PATHS ${Vulkan_INCLUDE_DIR} ${EFK_RUNTIME_ROOT}/EffekseerRendererVulkan)
endif ()

if (BUILD_METAL)
    list (APPEND MODULE_INCLUDE_PATHS ${EFK_RUNTIME_ROOT}/EffekseerRendererMetal)
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

set (MODULE_LIBRARIES
    Effekseer
    EffekseerRendererGL
    ${EFK_EXAMPLES_GLFW}
    LLGI
    Example_Utils
)

if (WIN32)
    list (APPEND MODULE_LIBRARIES EffekseerRendererDX11)
    list (APPEND MODULE_LIBRARIES EffekseerRendererDX12)
    list (APPEND MODULE_LIBRARIES EffekseerSoundXAudio2)
endif ()

if (BUILD_VULKAN)
    list (APPEND MODULE_LIBRARIES
        ${Vulkan_LIBRARIES}
        glslang
        glslang-default-resource-limits
        SPIRV
        OSDependent
        EffekseerRendererVulkan
    )
endif ()

if (APPLE)
    list (APPEND MODULE_LIBRARIES EffekseerRendererMetal)
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

LinkAppleLibs(${PROJECT_NAME})

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

