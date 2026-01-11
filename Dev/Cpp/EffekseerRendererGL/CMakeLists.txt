cmake_minimum_required(VERSION 3.15)
project(EffekseerRendererGL)

file(GLOB_RECURSE LOCAL_HEADERS_GL *.h)
file(GLOB_RECURSE LOCAL_SOURCES_GL *.cpp)
list(APPEND LOCAL_HEADERS_GL
     ../EffekseerMaterialCompiler/OpenGL/EffekseerMaterialCompilerGL.h)
list(APPEND LOCAL_SOURCES_GL
     ../EffekseerMaterialCompiler/OpenGL/EffekseerMaterialCompilerGL.cpp)
list(APPEND LOCAL_SOURCES_GL
     ../EffekseerMaterialCompiler/GLSLGenerator/ShaderGenerator.h)
list(APPEND LOCAL_SOURCES_GL
     ../EffekseerMaterialCompiler/GLSLGenerator/ShaderGenerator.cpp)
list(APPEND LOCAL_SOURCES_GL
     ../EffekseerMaterialCompiler/Common/ShaderGeneratorCommon.h)
list(APPEND LOCAL_SOURCES_GL
     ../EffekseerMaterialCompiler/Common/ShaderGeneratorCommon.cpp)

filterfolder("${LOCAL_HEADERS_GL}")
filterfolder("${LOCAL_SOURCES_GL}")

set(LOCAL_SOURCES ${LOCAL_HEADERS_GL} ${LOCAL_SOURCES_GL})

set(PublicHeaders
    EffekseerRendererGL.h EffekseerRendererGL/EffekseerRendererGL.Base.Pre.h
    EffekseerRendererGL/EffekseerRendererGL.Renderer.h)

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
