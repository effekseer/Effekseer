cmake_minimum_required(VERSION 3.15)
project(EffekseerRendererLLGI)

set(PublicHeaders EffekseerRendererLLGI/Common.h)

set(LocalHeaders
    EffekseerRendererLLGI/EffekseerRendererLLGI.Base.h
    EffekseerRendererLLGI/EffekseerRendererLLGI.Base.Pre.h
    EffekseerRendererLLGI/EffekseerRendererLLGI.GpuTimer.h
    EffekseerRendererLLGI/EffekseerRendererLLGI.GpuParticles.h
    EffekseerRendererLLGI/EffekseerRendererLLGI.MaterialLoader.h
    EffekseerRendererLLGI/EffekseerRendererLLGI.ModelRenderer.h
    EffekseerRendererLLGI/EffekseerRendererLLGI.Renderer.h
    EffekseerRendererLLGI/EffekseerRendererLLGI.RendererImplemented.h
    EffekseerRendererLLGI/EffekseerRendererLLGI.RenderState.h
    EffekseerRendererLLGI/EffekseerRendererLLGI.Shader.h
    EffekseerRendererLLGI/GraphicsDevice.h)

set(LocalSources
    EffekseerRendererLLGI/Common.cpp
    EffekseerRendererLLGI/EffekseerRendererLLGI.GpuTimer.cpp
    EffekseerRendererLLGI/EffekseerRendererLLGI.GpuParticles.cpp
    EffekseerRendererLLGI/EffekseerRendererLLGI.MaterialLoader.cpp
    EffekseerRendererLLGI/EffekseerRendererLLGI.ModelRenderer.cpp
    EffekseerRendererLLGI/EffekseerRendererLLGI.Renderer.cpp
    EffekseerRendererLLGI/EffekseerRendererLLGI.RenderState.cpp
    EffekseerRendererLLGI/EffekseerRendererLLGI.Shader.cpp
    EffekseerRendererLLGI/GraphicsDevice.cpp)

filterfolder("${PublicHeaders}")
filterfolder("${LocalHeaders}")
filterfolder("${LocalSources}")

add_library(${PROJECT_NAME} STATIC ${PublicHeaders} ${LocalHeaders}
                                   ${LocalSources})
target_include_directories(
  ${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../3rdparty/LLGI/src
                          ${EFK_THIRDPARTY_INCLUDES})
target_link_libraries(${PROJECT_NAME} PUBLIC Effekseer EffekseerRendererCommon
                                             LLGI)

target_include_directories(
  ${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                         $<INSTALL_INTERFACE:include>)

warnerror(${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER
                                                 "${PublicHeaders}")

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
