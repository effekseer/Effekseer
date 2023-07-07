cmake_minimum_required(VERSION 3.15)
project(EffekseerRendererLLGI)

set(PublicHeaders
    Common.h
)

set(LocalHeaders
    EffekseerRendererLLGI.Base.h
    EffekseerRendererLLGI.Base.Pre.h
    EffekseerRendererLLGI.MaterialLoader.h
    EffekseerRendererLLGI.ModelRenderer.h
    EffekseerRendererLLGI.Renderer.h
    EffekseerRendererLLGI.RendererImplemented.h
    EffekseerRendererLLGI.RenderState.h
    EffekseerRendererLLGI.Shader.h
    GraphicsDevice.h
)

set(LocalSources
    Common.cpp
    EffekseerRendererLLGI.MaterialLoader.cpp
    EffekseerRendererLLGI.ModelRenderer.cpp
    EffekseerRendererLLGI.Renderer.cpp
    EffekseerRendererLLGI.RenderState.cpp
    EffekseerRendererLLGI.Shader.cpp
    GraphicsDevice.cpp
)

FilterFolder("${PublicHeaders}")
FilterFolder("${LocalHeaders}")
FilterFolder("${LocalSources}")

add_library(${PROJECT_NAME} STATIC ${PublicHeaders} ${LocalHeaders} ${LocalSources})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/../Effekseer ${CMAKE_CURRENT_LIST_DIR}/../3rdparty/LLGI/src ${EFK_THIRDPARTY_INCLUDES})
target_link_libraries(${PROJECT_NAME} PUBLIC Effekseer EffekseerRendererCommon LLGI)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
    $<INSTALL_INTERFACE:include/EffekseerRendererLLGI>
)

WarnError(${PROJECT_NAME})

set_target_properties(
    ${PROJECT_NAME} PROPERTIES PUBLIC_HEADER 
     "${PublicHeaders}")

foreach (file ${PublicHeaders})
    get_filename_component(dir ${file} DIRECTORY)
    install(FILES ${file} DESTINATION include/EffekseerRendererLLGI/${dir})
endforeach()

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-export
    INCLUDES DESTINATION include/EffekseerRendererLLGI
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib)

install(
    EXPORT ${PROJECT_NAME}-export
    FILE ${PROJECT_NAME}-config.cmake
    DESTINATION lib/cmake
    EXPORT_LINK_INTERFACE_LIBRARIES)
