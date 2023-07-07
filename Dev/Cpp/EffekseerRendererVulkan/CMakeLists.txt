cmake_minimum_required (VERSION 3.0.0)
project(EffekseerRendererVulkan)

find_package(Vulkan REQUIRED)

file(GLOB_RECURSE LOCAL_HEADERS_Vulkan *.h)
file(GLOB_RECURSE LOCAL_SOURCES_Vulkan *.cpp)
list(APPEND LOCAL_SOURCES_Vulkan ../EffekseerMaterialCompiler/GLSLGenerator/ShaderGenerator.h)
list(APPEND LOCAL_SOURCES_Vulkan ../EffekseerMaterialCompiler/GLSLGenerator/ShaderGenerator.cpp)
list(APPEND LOCAL_SOURCES_Vulkan ../EffekseerMaterialCompiler/Common/ShaderGeneratorCommon.h)
list(APPEND LOCAL_SOURCES_Vulkan ../EffekseerMaterialCompiler/Common/ShaderGeneratorCommon.cpp)

FilterFolder("${LOCAL_HEADERS_Vulkan}")
FilterFolder("${LOCAL_SOURCES_Vulkan}")

set(LOCAL_SOURCES
    ${LOCAL_HEADERS_Vulkan}
    ${LOCAL_SOURCES_Vulkan})

set(PublicHeaders
    EffekseerRendererVulkan.h
    EffekseerRenderer/EffekseerRendererVulkan.Base.Pre.h
    EffekseerRenderer/EffekseerRendererVulkan.Renderer.h)

#--------------------
# Projects

add_library(${PROJECT_NAME} STATIC ${LOCAL_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../Effekseer ${CMAKE_CURRENT_LIST_DIR}/../3rdparty/LLGI/src ${EFK_THIRDPARTY_INCLUDES})
target_include_directories(${PROJECT_NAME} PRIVATE ${Vulkan_INCLUDE_DIRS})
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${PublicHeaders}")
target_link_libraries(${PROJECT_NAME} PUBLIC Effekseer EffekseerRendererCommon EffekseerRendererLLGI LLGI)

if(CLANG_FORMAT_ENABLED)
    clang_format(${PROJECT_NAME})
endif()

# WarnError(${PROJECT_NAME})

#--------------------
# Install

foreach (file ${PublicHeaders})
    get_filename_component(dir ${file} DIRECTORY)
    install(FILES ${file} DESTINATION include/EffekseerRendererVulkan/${dir})
endforeach()

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-export
    INCLUDES DESTINATION include/EffekseerRendererVulkan
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib)

install(
    EXPORT ${PROJECT_NAME}-export
    FILE ${PROJECT_NAME}-config.cmake
    DESTINATION lib/cmake
    EXPORT_LINK_INTERFACE_LIBRARIES)
