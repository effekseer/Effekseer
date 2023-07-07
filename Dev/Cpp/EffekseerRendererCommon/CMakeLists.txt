project(EffekseerRendererCommon)
cmake_minimum_required(VERSION 3.1)

# Setup source codes

set(PublicHeaders
    EffekseerRenderer.Renderer.h
    EffekseerRenderer.RenderStateBase.h
    EffekseerRenderer.ModelRendererBase.h
    EffekseerRenderer.RibbonRendererBase.h
    EffekseerRenderer.RingRendererBase.h
    EffekseerRenderer.SpriteRendererBase.h
    EffekseerRenderer.StandardRenderer.h
    EffekseerRenderer.TrackRendererBase.h
    EffekseerRenderer.CommonUtils.h
    EffekseerRenderer.ShaderBase.h
    EffekseerRenderer.Renderer_Impl.h
    GraphicsDeviceCPU.h
    ModelLoader.h
    TextureLoader.h
    VertexBuffer.h
)

set(LocalHeaders
    EffekseerRenderer.CommonUtils.h
    EffekseerRenderer.DDSTextureLoader.h
    EffekseerRenderer.PngTextureLoader.h
    EffekseerRenderer.TGATextureLoader.h
)

set(LocalSources
    EffekseerRenderer.CommonUtils.cpp
    EffekseerRenderer.DDSTextureLoader.cpp
    EffekseerRenderer.ModelRendererBase.cpp
    EffekseerRenderer.PngTextureLoader.cpp
    EffekseerRenderer.Renderer.cpp
    EffekseerRenderer.Renderer_Impl.cpp
    EffekseerRenderer.RenderStateBase.cpp
    EffekseerRenderer.RibbonRendererBase.cpp
    EffekseerRenderer.RingRendererBase.cpp
    EffekseerRenderer.SpriteRendererBase.cpp
    EffekseerRenderer.TGATextureLoader.cpp
    EffekseerRenderer.TrackRendererBase.cpp
    GraphicsDeviceCPU.cpp
    ModelLoader.cpp
    TextureLoader.cpp
    VertexBuffer.cpp
)

FilterFolder("${PublicHeaders}")
FilterFolder("${LocalHeaders}")
FilterFolder("${LocalSources}")

add_library(${PROJECT_NAME} STATIC ${PublicHeaders} ${LocalHeaders} ${LocalSources})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/../Effekseer ${EFK_THIRDPARTY_INCLUDES})
target_link_libraries(${PROJECT_NAME} PUBLIC Effekseer)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
    $<INSTALL_INTERFACE:include/EffekseerRendererCommon>
)

if(USE_LIBPNG_LOADER)
    add_dependencies(${PROJECT_NAME} ExternalProject_zlib ExternalProject_libpng) 

    target_link_libraries(
        ${PROJECT_NAME}
        PRIVATE
        debug "${LIBPNG_LIB_DEBUG}"
        optimized "${LIBPNG_LIB_RELEASE}"
        debug "${ZLIB_LIB_DEBUG}"
        optimized "${ZLIB_LIB_RELEASE}"
    )
endif()

WarnError(${PROJECT_NAME})

set_target_properties(
    ${PROJECT_NAME} PROPERTIES PUBLIC_HEADER 
     "${PublicHeaders}")

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-export
    INCLUDES DESTINATION include/EffekseerRendererCommon
    PUBLIC_HEADER DESTINATION include/EffekseerRendererCommon
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib)

install(
    EXPORT ${PROJECT_NAME}-export
    FILE ${PROJECT_NAME}-config.cmake
    DESTINATION lib/cmake
    EXPORT_LINK_INTERFACE_LIBRARIES)
