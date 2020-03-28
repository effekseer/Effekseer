cmake_minimum_required (VERSION 3.0.0)
project(RuntimeExample_Vulkan)
set(CMAKE_CXX_STANDARD 17)

find_package(Vulkan REQUIRED)

file(GLOB LOCAL_SOURCES "*.h" "*.cpp")

add_definitions(-DEFK_EXAMPLE_VULKAN_ASSETS_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/../Resources/\")
add_definitions(-DEFK_EXAMPLE_VULKAN_ASSETS_DIR_U16=u\"${CMAKE_CURRENT_SOURCE_DIR}/../Resources/\")

add_executable(${PROJECT_NAME} ${LOCAL_SOURCES})

target_include_directories(
    ${PROJECT_NAME} PRIVATE
    ${Vulkan_INCLUDE_DIRS}
    ${EFK_RUNTIME_ROOT}/Effekseer
    ${EFK_RUNTIME_ROOT}/3rdParty/LLGI/src
    ${EFK_THIRDPARTY_INCLUDES}
)

target_link_directories(
    ${PROJECT_NAME} PRIVATE
    ${EFK_THIRDPARTY_LIBRARY_DIRECTORIES}
)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    ${Vulkan_LIBRARIES}
    Effekseer
    LLGI
    EffekseerRendererVulkan
    debug glfw3d
    optimized glfw3
)


if(USE_LIBPNG_LOADER)
    target_link_directories(
        ${PROJECT_NAME}
        PRIVATE
	    ${EFK_THIRDPARTY_LIBRARY_DIRECTORIES}
    )

    target_link_libraries(
        ${PROJECT_NAME}
        PRIVATE
        debug "${ZLIB_LIB_DEBUG}"
        optimized "${ZLIB_LIB_RELEASE}"
        debug "${LIBPNG_LIB_DEBUG}"
	    optimized "${LIBPNG_LIB_RELEASE}"
	)
endif()