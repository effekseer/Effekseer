cmake_minimum_required (VERSION 3.0.0)

project(Example_Vulkan)

find_package(Vulkan REQUIRED)

add_executable(Example_Vulkan main.cpp)

target_include_directories(
    Example_Vulkan
    PRIVATE
    ${Vulkan_INCLUDE_DIRS}
    ${EFK_RUNTIME_ROOT}/Effekseer
    ${EFK_RUNTIME_ROOT}/EffekseerRendererVulkan
    ${EFK_RUNTIME_ROOT}/3rdParty/LLGI/src/
    ../Utils
)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    Effekseer
    LLGI
    EffekseerRendererVulkan
    Example_Utils
    ${Vulkan_LIBRARIES}
)

# TODO Improve it
target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    glslang
    SPIRV
    OGLCompiler
    OSDependent
)

target_link_directories(
    ${PROJECT_NAME}
    PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}/../../Dev/Cpp/3rdParty/LLGI/EP/Install/glslang/lib
)

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