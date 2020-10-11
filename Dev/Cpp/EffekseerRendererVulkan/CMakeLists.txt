cmake_minimum_required (VERSION 3.0.0)
project(EffekseerRendererVulkan)

find_package(Vulkan REQUIRED)

#--------------------
# Files

file(GLOB_RECURSE LOCAL_SOURCES_Common ../EffekseerRendererCommon/*.h ../EffekseerRendererCommon/*.cpp)

list(APPEND LOCAL_SOURCES_Common 
    ../EffekseerRendererCommon/TextureLoader.h
    ../EffekseerRendererCommon/TextureLoader.cpp)

source_group("EffekseerRendererCommon" FILES ${LOCAL_SOURCES_Common})

file(GLOB_RECURSE LOCAL_HEADERS_Vulkan *.h)
file(GLOB_RECURSE LOCAL_SOURCES_Vulkan *.cpp)
FilterFolder("${LOCAL_HEADERS_Vulkan}")
FilterFolder("${LOCAL_SOURCES_Vulkan}")

file(GLOB_RECURSE LOCAL_SOURCES_LLGI
    ../EffekseerRendererLLGI/*.h
    ../EffekseerRendererLLGI/*.cpp)

source_group("EffekseerRendererLLGI" FILES ${LOCAL_SOURCES_LLGI})

set(LOCAL_SOURCES
    ${LOCAL_SOURCES_Common}
    ${LOCAL_HEADERS_Vulkan}
    ${LOCAL_SOURCES_Vulkan}
    ${LOCAL_SOURCES_LLGI})

#--------------------
# Projects

add_library(${PROJECT_NAME} STATIC ${LOCAL_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../Effekseer ${CMAKE_CURRENT_LIST_DIR}/../3rdparty/LLGI/src ${EFK_THIRDPARTY_INCLUDES})
target_include_directories(${PROJECT_NAME} PRIVATE ${Vulkan_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} LLGI)
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${LOCAL_HEADERS_Vulkan}")
add_dependencies(${PROJECT_NAME} Effekseer)

if(USE_LIBPNG_LOADER AND USE_INTERNAL_LOADER)
    add_dependencies(${PROJECT_NAME} ExternalProject_zlib ExternalProject_libpng) 
endif()

#--------------------
# Install

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-export
    INCLUDES DESTINATION include/EffekseerRendererVulkan
    PUBLIC_HEADER DESTINATION include/EffekseerRendererVulkan
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib)

install(
    EXPORT ${PROJECT_NAME}-export
    FILE ${PROJECT_NAME}-config.cmake
    DESTINATION lib/cmake
    EXPORT_LINK_INTERFACE_LIBRARIES)
