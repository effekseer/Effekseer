cmake_minimum_required(VERSION 3.15)
project(EffekseerRendererDX9)

file(GLOB_RECURSE LOCAL_HEADERS_DX9 *.h)
file(GLOB_RECURSE LOCAL_SOURCES_DX9 *.cpp)
list(APPEND LOCAL_HEADERS_DX9
     ../EffekseerMaterialCompiler/DirectX9/EffekseerMaterialCompilerDX9.h)
list(APPEND LOCAL_SOURCES_DX9
     ../EffekseerMaterialCompiler/DirectX9/EffekseerMaterialCompilerDX9.cpp)
list(APPEND LOCAL_SOURCES_DX9
     ../EffekseerMaterialCompiler/HLSLGenerator/ShaderGenerator.h)
list(APPEND LOCAL_SOURCES_DX9
     ../EffekseerMaterialCompiler/HLSLGenerator/ShaderGenerator.cpp)
list(APPEND LOCAL_SOURCES_DX9
     ../EffekseerMaterialCompiler/Common/ShaderGeneratorCommon.h)
list(APPEND LOCAL_SOURCES_DX9
     ../EffekseerMaterialCompiler/Common/ShaderGeneratorCommon.cpp)

filterfolder("${LOCAL_HEADERS_DX9}")
filterfolder("${LOCAL_SOURCES_DX9}")

set(LOCAL_SOURCES ${LOCAL_HEADERS_DX9} ${LOCAL_SOURCES_DX9})

set(PublicHeaders
    EffekseerRendererDX9.h EffekseerRendererDX9/EffekseerRendererDX9.Base.Pre.h
    EffekseerRendererDX9/EffekseerRendererDX9.Renderer.h)

# --------------------
# Projects

add_library(${PROJECT_NAME} STATIC ${LOCAL_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${EFK_THIRDPARTY_INCLUDES})
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER
                                                 "${PublicHeader}")
target_link_libraries(${PROJECT_NAME} PUBLIC Effekseer EffekseerRendererCommon)

target_include_directories(
  ${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                         $<INSTALL_INTERFACE:include>)

if(CLANG_FORMAT_ENABLED)
  clang_format(${PROJECT_NAME})
endif()

if(USE_LIBPNG_LOADER)
  add_dependencies(${PROJECT_NAME} ExternalProject_zlib ExternalProject_libpng)
endif()

# --------------------
# Install

foreach(file ${PublicHeaders})
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
