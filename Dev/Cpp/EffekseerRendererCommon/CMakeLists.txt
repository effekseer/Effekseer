project(EffekseerRendererCommon)
cmake_minimum_required(VERSION 3.1)

if(BUILD_UNITYPLUGIN OR BUILD_UNITYPLUGIN_FOR_IOS)

if(BUILD_UNITYPLUGIN)
    add_library(${PROJECT_NAME} INTERFACE)
endif()

# Setup source codes

set(PublicHeader
    EffekseerRenderer.Renderer.h
    EffekseerRenderer.VertexBufferBase.h
    EffekseerRenderer.IndexBufferBase.h
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
)

if(BUILD_UNITYPLUGIN_FOR_IOS)
    file(GLOB_RECURSE LOCAL_SOURCES *.cpp)
    add_library(${PROJECT_NAME} STATIC ${LOCAL_SOURCES})
    target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/../Effekseer ${EFK_THIRDPARTY_INCLUDES})
    target_link_libraries(${PROJECT_NAME} PUBLIC Effekseer)
endif()

set_target_properties(
    ${PROJECT_NAME} PROPERTIES PUBLIC_HEADER 
     "${PublicHeader}")

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

endif()