cmake_minimum_required (VERSION 3.10)
project(EffekseerRendererDX11)

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
    ${LOCAL_HEADERS_DX11}
    ${LOCAL_SOURCES_DX11})

set(PublicHeaders
    EffekseerRendererDX11.h
    EffekseerRendererDX11/EffekseerRendererDX11.Base.h
    EffekseerRendererDX11/EffekseerRendererDX11.Base.Pre.h
    EffekseerRendererDX11/EffekseerRendererDX11.Renderer.h)

#--------------------
# Projects

add_library(${PROJECT_NAME} STATIC ${LOCAL_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${EFK_THIRDPARTY_INCLUDES})
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${PublicHeaders}")
target_link_libraries(${PROJECT_NAME} PUBLIC Effekseer EffekseerRendererCommon)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>)

if(CLANG_FORMAT_ENABLED)
    clang_format(${PROJECT_NAME})
endif()

WarnError(${PROJECT_NAME})

#--------------------
# Install

foreach (file ${PublicHeaders})
    get_filename_component(dir ${file} DIRECTORY)
    install(FILES ${file} DESTINATION include/${dir})
endforeach()

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-export
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib)

install(
    EXPORT ${PROJECT_NAME}-export
    FILE ${PROJECT_NAME}-config.cmake
    DESTINATION lib/cmake
    EXPORT_LINK_INTERFACE_LIBRARIES)
