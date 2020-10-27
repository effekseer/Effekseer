cmake_minimum_required (VERSION 3.0.0)
project(EffekseerRendererDX12)

#--------------------
# Files

file(GLOB_RECURSE LOCAL_SOURCES_Common ../EffekseerRendererCommon/*.h ../EffekseerRendererCommon/*.cpp)
source_group("EffekseerRendererCommon" FILES ${LOCAL_SOURCES_Common})

file(GLOB_RECURSE LOCAL_HEADERS_DX12 *.h)
file(GLOB_RECURSE LOCAL_SOURCES_DX12 *.cpp)
list(APPEND LOCAL_HEADERS_DX12 ../EffekseerMaterialCompiler/DirectX12/EffekseerMaterialCompilerDX12.h)
list(APPEND LOCAL_SOURCES_DX12 ../EffekseerMaterialCompiler/DirectX12/EffekseerMaterialCompilerDX12.cpp)

FilterFolder("${LOCAL_HEADERS_DX12}")
FilterFolder("${LOCAL_SOURCES_DX12}")

file(GLOB_RECURSE LOCAL_SOURCES_LLGI
    ../EffekseerRendererLLGI/*.h
    ../EffekseerRendererLLGI/*.cpp)

source_group("EffekseerRendererLLGI" FILES ${LOCAL_SOURCES_LLGI})

set(LOCAL_SOURCES
    ${LOCAL_SOURCES_Common}
    ${LOCAL_HEADERS_DX12}
    ${LOCAL_SOURCES_DX12}
    ${LOCAL_SOURCES_LLGI})

#--------------------
# Projects

add_library(${PROJECT_NAME} STATIC ${LOCAL_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../Effekseer ${CMAKE_CURRENT_LIST_DIR}/../3rdparty/LLGI/src ${EFK_THIRDPARTY_INCLUDES})
target_link_libraries(${PROJECT_NAME} LLGI)
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${LOCAL_HEADERS_DX12}")
add_dependencies(${PROJECT_NAME} Effekseer)

if(CLANG_FORMAT_ENABLED)
    clang_format(${PROJECT_NAME})
endif()

if(USE_LIBPNG_LOADER AND USE_INTERNAL_LOADER)
    add_dependencies(${PROJECT_NAME} ExternalProject_zlib ExternalProject_libpng) 
endif()

#--------------------
# Install

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-export
    INCLUDES DESTINATION include/EffekseerRendererDX12
    PUBLIC_HEADER DESTINATION include/EffekseerRendererDX12
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib)

install(
    EXPORT ${PROJECT_NAME}-export
    FILE ${PROJECT_NAME}-config.cmake
    DESTINATION lib/cmake
    EXPORT_LINK_INTERFACE_LIBRARIES)
