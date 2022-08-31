cmake_minimum_required (VERSION 3.0.0)
project(EffekseerRendererGL)

#--------------------
# Files

file(GLOB_RECURSE LOCAL_SOURCES_Common ../EffekseerRendererCommon/*.h ../EffekseerRendererCommon/*.cpp)

list(APPEND LOCAL_SOURCES_Common 
    ../EffekseerRendererCommon/TextureLoader.h
    ../EffekseerRendererCommon/TextureLoader.cpp)

source_group("EffekseerRendererCommon" FILES ${LOCAL_SOURCES_Common})

file(GLOB_RECURSE LOCAL_HEADERS_GL *.h)
file(GLOB_RECURSE LOCAL_SOURCES_GL *.cpp)
list(APPEND LOCAL_HEADERS_GL ../EffekseerMaterialCompiler/OpenGL/EffekseerMaterialCompilerGL.h)
list(APPEND LOCAL_SOURCES_GL ../EffekseerMaterialCompiler/OpenGL/EffekseerMaterialCompilerGL.cpp)
list(APPEND LOCAL_SOURCES_GL ../EffekseerMaterialCompiler/GLSLGenerator/ShaderGenerator.h)
list(APPEND LOCAL_SOURCES_GL ../EffekseerMaterialCompiler/GLSLGenerator/ShaderGenerator.cpp)
list(APPEND LOCAL_SOURCES_GL ../EffekseerMaterialCompiler/Common/ShaderGeneratorCommon.h)
list(APPEND LOCAL_SOURCES_GL ../EffekseerMaterialCompiler/Common/ShaderGeneratorCommon.cpp)

FilterFolder("${LOCAL_HEADERS_GL}")
FilterFolder("${LOCAL_SOURCES_GL}")

set(LOCAL_SOURCES
    ${LOCAL_SOURCES_Common}
    ${LOCAL_HEADERS_GL}
    ${LOCAL_SOURCES_GL})

set(PublicHeader
    EffekseerRendererGL.h)

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

#--------------------
# Install

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-export
    INCLUDES DESTINATION include/EffekseerRendererGL
    PUBLIC_HEADER DESTINATION include/EffekseerRendererGL
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib)

install(
    EXPORT ${PROJECT_NAME}-export
    FILE ${PROJECT_NAME}-config.cmake
    DESTINATION lib/cmake
    EXPORT_LINK_INTERFACE_LIBRARIES)
