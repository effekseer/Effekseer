cmake_minimum_required (VERSION 3.0.0)
project(RuntimeExample_Vulkan)
set(CMAKE_CXX_STANDARD 17)

find_package(Vulkan REQUIRED)

file(GLOB LOCAL_SOURCES "*.h" "*.cpp")

add_definitions(-DEFK_EXAMPLE_VULKAN_ASSETS_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/assets/\")
add_definitions(-DEFK_EXAMPLE_VULKAN_ASSETS_DIR_U16=u\"${CMAKE_CURRENT_SOURCE_DIR}/assets/\")

add_executable(${PROJECT_NAME} ${LOCAL_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${Vulkan_INCLUDE_DIRS} "${CMAKE_CURRENT_SOURCE_DIR}/../../Dev/Cpp/Effekseer" "${CMAKE_CURRENT_SOURCE_DIR}/../../Dev/Cpp/3rdParty/LLGI/src")
target_link_libraries(${PROJECT_NAME} ${Vulkan_LIBRARIES} Effekseer LLGI EffekseerRendererVulkan)


