cmake_minimum_required (VERSION 3.0.0)
project(EffekseerRendererDX11)

#--------------------
# Files

file(GLOB_RECURSE LOCAL_SOURCES_Common ../EffekseerRendererCommon/*.h ../EffekseerRendererCommon/*.cpp)

list(APPEND LOCAL_SOURCES_Common 
    ../EffekseerRendererCommon/TextureLoader.h
    ../EffekseerRendererCommon/TextureLoader.cpp)

source_group("EffekseerRendererCommon" FILES ${LOCAL_SOURCES_Common})

file(GLOB_RECURSE LOCAL_HEADERS_DX11 *.h)
file(GLOB_RECURSE LOCAL_SOURCES_DX11 *.cpp)
list(APPEND LOCAL_HEADERS_DX11 ../EffekseerMaterialCompiler/DirectX11/EffekseerMaterialCompilerDX11.h)
list(APPEND LOCAL_SOURCES_DX11 ../EffekseerMaterialCompiler/DirectX11/EffekseerMaterialCompilerDX11.cpp)
list(APPEND LOCAL_SOURCES_DX11 ../EffekseerMaterialCompiler/HLSLGenerator/ShaderGenerator.h)
list(APPEND LOCAL_SOURCES_DX11 ../EffekseerMaterialCompiler/HLSLGenerator/ShaderGenerator.cpp)
list(APPEND LOCAL_SOURCES_DX11 ../EffekseerMaterialCompiler/Common/ShaderGeneratorCommon.h)
list(APPEND LOCAL_SOURCES_DX11 ../EffekseerMaterialCompiler/Common/ShaderGeneratorCommon.cpp)

FilterFolder("${LOCAL_HEADERS_DX11}")
FilterFolder("${LOCAL_SOURCES_DX11}")

set(LOCAL_SOURCES
    ${LOCAL_SOURCES_Common}
    ${LOCAL_HEADERS_DX11}
    ${LOCAL_SOURCES_DX11})

set(PublicHeader
    EffekseerRendererDX11.h)

#--------------------
# Projects

add_library(${PROJECT_NAME} STATIC ${LOCAL_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/../Effekseer ${EFK_THIRDPARTY_INCLUDES})
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${PublicHeader}")
target_link_libraries(${PROJECT_NAME} PUBLIC Effekseer)

if(CLANG_FORMAT_ENABLED)
    clang_format(${PROJECT_NAME})
endif()

if(USE_LIBPNG_LOADER)
    add_dependencies(${PROJECT_NAME} ExternalProject_zlib ExternalProject_libpng) 
endif()

WarnError(${PROJECT_NAME})

#--------------------
# Install

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-export
    INCLUDES DESTINATION include/EffekseerRendererDX11
    PUBLIC_HEADER DESTINATION include/EffekseerRendererDX11
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib)

install(
    EXPORT ${PROJECT_NAME}-export
    FILE ${PROJECT_NAME}-config.cmake
    DESTINATION lib/cmake
    EXPORT_LINK_INTERFACE_LIBRARIES)
