cmake_minimum_required(VERSION 3.15)
project(EffekseerRendererDX12)

file(GLOB_RECURSE LOCAL_HEADERS_DX12 *.h)
file(GLOB_RECURSE LOCAL_SOURCES_DX12 *.cpp)
list(APPEND LOCAL_HEADERS_DX12 ../EffekseerMaterialCompiler/DirectX12/EffekseerMaterialCompilerDX12.h)
list(APPEND LOCAL_SOURCES_DX12 ../EffekseerMaterialCompiler/DirectX12/EffekseerMaterialCompilerDX12.cpp)
list(APPEND LOCAL_SOURCES_DX12 ../EffekseerMaterialCompiler/HLSLGenerator/ShaderGenerator.h)
list(APPEND LOCAL_SOURCES_DX12 ../EffekseerMaterialCompiler/HLSLGenerator/ShaderGenerator.cpp)
list(APPEND LOCAL_SOURCES_DX12 ../EffekseerMaterialCompiler/Common/ShaderGeneratorCommon.h)
list(APPEND LOCAL_SOURCES_DX12 ../EffekseerMaterialCompiler/Common/ShaderGeneratorCommon.cpp)

FilterFolder("${LOCAL_HEADERS_DX12}")
FilterFolder("${LOCAL_SOURCES_DX12}")

set(LOCAL_SOURCES
    ${LOCAL_HEADERS_DX12}
    ${LOCAL_SOURCES_DX12})

set(PublicHeaders
    EffekseerRendererDX12.h
    EffekseerRenderer/EffekseerRendererDX12.Base.Pre.h
    EffekseerRenderer/EffekseerRendererDX12.Renderer.h)

#--------------------
# Projects

add_library(${PROJECT_NAME} STATIC ${LOCAL_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../Effekseer ${CMAKE_CURRENT_LIST_DIR}/../3rdparty/LLGI/src ${EFK_THIRDPARTY_INCLUDES})
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${PublicHeaders}")
target_link_libraries(${PROJECT_NAME} PUBLIC Effekseer EffekseerRendererCommon EffekseerRendererLLGI LLGI)

if(CLANG_FORMAT_ENABLED)
    clang_format(${PROJECT_NAME})
endif()

WarnError(${PROJECT_NAME})

#--------------------
# Install

foreach (file ${PublicHeaders})
    get_filename_component(dir ${file} DIRECTORY)
    install(FILES ${file} DESTINATION include/EffekseerRendererDX12/${dir})
endforeach()

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-export
    INCLUDES DESTINATION include/EffekseerRendererDX12
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib)

install(
    EXPORT ${PROJECT_NAME}-export
    FILE ${PROJECT_NAME}-config.cmake
    DESTINATION lib/cmake
    EXPORT_LINK_INTERFACE_LIBRARIES)
