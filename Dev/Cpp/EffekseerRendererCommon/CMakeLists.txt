project(EffekseerRendererCommon)
cmake_minimum_required(VERSION 3.10)

# Setup source codes

set(PublicHeaders
    EffekseerRendererCommon/EffekseerRenderer.Renderer.h
    EffekseerRendererCommon/EffekseerRenderer.RenderStateBase.h
    EffekseerRendererCommon/EffekseerRenderer.ModelRendererBase.h
    EffekseerRendererCommon/EffekseerRenderer.RibbonRendererBase.h
    EffekseerRendererCommon/EffekseerRenderer.RingRendererBase.h
    EffekseerRendererCommon/EffekseerRenderer.SpriteRendererBase.h
    EffekseerRendererCommon/EffekseerRenderer.StandardRenderer.h
    EffekseerRendererCommon/EffekseerRenderer.TrackRendererBase.h
    EffekseerRendererCommon/EffekseerRenderer.CommonUtils.h
    EffekseerRendererCommon/EffekseerRenderer.GpuParticles.h
    EffekseerRendererCommon/EffekseerRenderer.ShaderBase.h
    EffekseerRendererCommon/EffekseerRenderer.Renderer_Impl.h
    EffekseerRendererCommon/GraphicsDeviceCPU.h
    EffekseerRendererCommon/ModelLoader.h
    EffekseerRendererCommon/TextureLoader.h
    EffekseerRendererCommon/TrailRenderer.h
    EffekseerRendererCommon/VertexBuffer.h)

set(LocalHeaders
    EffekseerRendererCommon/EffekseerRendererFlags.h
    EffekseerRendererCommon/EffekseerRenderer.CommonUtils.h
    EffekseerRendererCommon/EffekseerRenderer.DDSTextureLoader.h
    EffekseerRendererCommon/EffekseerRenderer.PngTextureLoader.h
    EffekseerRendererCommon/EffekseerRenderer.TGATextureLoader.h)

set(LocalSources
    EffekseerRendererCommon/EffekseerRenderer.CommonUtils.cpp
    EffekseerRendererCommon/EffekseerRenderer.DDSTextureLoader.cpp
    EffekseerRendererCommon/EffekseerRenderer.GpuParticles.cpp
    EffekseerRendererCommon/EffekseerRenderer.ModelRendererBase.cpp
    EffekseerRendererCommon/EffekseerRenderer.PngTextureLoader.cpp
    EffekseerRendererCommon/EffekseerRenderer.Renderer.cpp
    EffekseerRendererCommon/EffekseerRenderer.Renderer_Impl.cpp
    EffekseerRendererCommon/EffekseerRenderer.RenderStateBase.cpp
    EffekseerRendererCommon/EffekseerRenderer.RibbonRendererBase.cpp
    EffekseerRendererCommon/EffekseerRenderer.RingRendererBase.cpp
    EffekseerRendererCommon/EffekseerRenderer.SpriteRendererBase.cpp
    EffekseerRendererCommon/EffekseerRenderer.TGATextureLoader.cpp
    EffekseerRendererCommon/EffekseerRenderer.TrackRendererBase.cpp
    EffekseerRendererCommon/GraphicsDeviceCPU.cpp
    EffekseerRendererCommon/ModelLoader.cpp
    EffekseerRendererCommon/TextureLoader.cpp
    EffekseerRendererCommon/VertexBuffer.cpp)

filterfolder("${PublicHeaders}")
filterfolder("${LocalHeaders}")
filterfolder("${LocalSources}")

add_library(${PROJECT_NAME} STATIC ${PublicHeaders} ${LocalHeaders}
                                   ${LocalSources})
target_include_directories(${PROJECT_NAME} PRIVATE ${EFK_THIRDPARTY_INCLUDES})
target_link_libraries(${PROJECT_NAME} PUBLIC Effekseer)

target_include_directories(
  ${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                         $<INSTALL_INTERFACE:include>)

if(USE_LIBPNG_LOADER)
  add_dependencies(${PROJECT_NAME} ExternalProject_zlib ExternalProject_libpng)

  target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE debug
            "${LIBPNG_LIB_DEBUG}"
            optimized
            "${LIBPNG_LIB_RELEASE}"
            debug
            "${ZLIB_LIB_DEBUG}"
            optimized
            "${ZLIB_LIB_RELEASE}")
endif()

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
