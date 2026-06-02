cmake_minimum_required(VERSION 3.15)

project(EffekseerToolRuntime CXX)

set(srcs
    EffekseerToolRuntime/BackgroundPlaneRenderer.h
    EffekseerToolRuntime/BackgroundPlaneRenderer.cpp
    EffekseerToolRuntime/DepthRendering.h
    EffekseerToolRuntime/DepthRendering.cpp
    EffekseerToolRuntime/GroundRendering.h
    EffekseerToolRuntime/GroundRendering.cpp
    EffekseerToolRuntime/GroundPlaneRenderer.h
    EffekseerToolRuntime/GroundPlaneRenderer.cpp
    EffekseerToolRuntime/Image.h
    EffekseerToolRuntime/ImageRenderer.h
    EffekseerToolRuntime/ImageRenderer.cpp
    EffekseerToolRuntime/LineRenderer.h
    EffekseerToolRuntime/LineRenderer.cpp
    EffekseerToolRuntime/PostEffects.h
    EffekseerToolRuntime/PostEffects.cpp
    EffekseerToolRuntime/PostProcess.h
    EffekseerToolRuntime/PostProcess.cpp
    EffekseerToolRuntime/RenderImage.h
    EffekseerToolRuntime/RenderImage.cpp
    EffekseerToolRuntime/StaticMeshRenderer.h
    EffekseerToolRuntime/StaticMeshRenderer.cpp)

if(TARGET EffekseerRendererLLGI)
  list(APPEND srcs EffekseerToolRuntime/LLGIShaderCompiler.h
       EffekseerToolRuntime/LLGIShaderCompiler.cpp)
endif()

add_library(${PROJECT_NAME} STATIC ${srcs})

target_include_directories(${PROJECT_NAME} PUBLIC ./ ../Effekseer/
                                                ../EffekseerRendererCommon/)
target_link_libraries(${PROJECT_NAME} PUBLIC Effekseer EffekseerRendererCommon)

if(TARGET EffekseerRendererLLGI)
  target_include_directories(${PROJECT_NAME}
                             PRIVATE ../3rdParty/LLGI/src)
  target_compile_definitions(${PROJECT_NAME}
                             PRIVATE EFFEKSEER_TOOL_RUNTIME_WITH_LLGI)
  target_link_libraries(${PROJECT_NAME} PUBLIC EffekseerRendererLLGI LLGI)

  if(BUILD_VULKAN)
    find_package(Vulkan REQUIRED)
    target_include_directories(${PROJECT_NAME} PRIVATE ${Vulkan_INCLUDE_DIRS})
  endif()
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

filterfolder("${srcs}")

if(CLANG_FORMAT_ENABLED)
  clang_format(${PROJECT_NAME})
endif()
