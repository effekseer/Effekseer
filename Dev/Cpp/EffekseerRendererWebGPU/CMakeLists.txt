cmake_minimum_required(VERSION 3.15)
project(EffekseerRendererWebGPU)

file(GLOB_RECURSE LOCAL_HEADERS_WebGPU *.h)
file(GLOB_RECURSE LOCAL_SOURCES_WebGPU *.cpp)

if(BUILD_WEBGPU_BROWSER_TEST)
  list(FILTER LOCAL_SOURCES_WebGPU EXCLUDE REGEX
       "EffekseerMaterialCompilerWebGPU\\.cpp$")
endif()

list(APPEND LOCAL_HEADERS_WebGPU
     ../EffekseerMaterialCompiler/HLSLGenerator/ShaderGenerator.h
     ../EffekseerMaterialCompiler/Common/ShaderGeneratorCommon.h)
list(APPEND LOCAL_SOURCES_WebGPU
     ../EffekseerMaterialCompiler/HLSLGenerator/ShaderGenerator.cpp
     ../EffekseerMaterialCompiler/Common/ShaderGeneratorCommon.cpp)

filterfolder("${LOCAL_HEADERS_WebGPU}")
filterfolder("${LOCAL_SOURCES_WebGPU}")

set(LOCAL_SOURCES ${LOCAL_HEADERS_WebGPU} ${LOCAL_SOURCES_WebGPU})

set(PublicHeaders
    EffekseerRendererWebGPU.h
    EffekseerRendererWebGPU/EffekseerRendererWebGPU.Base.Pre.h
    EffekseerRendererWebGPU/EffekseerRendererWebGPU.Renderer.h)

add_library(${PROJECT_NAME} STATIC ${LOCAL_SOURCES})
target_include_directories(
  ${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../3rdParty/LLGI/src
                          ${CMAKE_CURRENT_LIST_DIR}/../EffekseerRendererVulkan
                          ${EFK_THIRDPARTY_INCLUDES})

if(TARGET dawn::webgpu_dawn)
  target_link_libraries(${PROJECT_NAME} PUBLIC dawn::webgpu_dawn)
elseif(TARGET webgpu_dawn)
  target_link_libraries(${PROJECT_NAME} PUBLIC webgpu_dawn)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER
                                                 "${PublicHeaders}")
target_link_libraries(${PROJECT_NAME} PUBLIC Effekseer EffekseerRendererCommon
                                             EffekseerRendererLLGI LLGI)
if(NOT BUILD_WEBGPU_BROWSER_TEST)
  target_link_libraries(${PROJECT_NAME} PRIVATE
                        $<BUILD_INTERFACE:ShaderTranspilerCore>)
endif()

target_include_directories(
  ${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                         $<INSTALL_INTERFACE:include>)

if(CLANG_FORMAT_ENABLED)
  clang_format(${PROJECT_NAME})
endif()

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
