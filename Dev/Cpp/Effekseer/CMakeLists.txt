project(Effekseer)
cmake_minimum_required(VERSION 3.1)

# Setup source codes
file(GLOB effekseer_h
    *.h
    Effekseer/*.h
    Effekseer/Renderer/*.h
    Effekseer/Culling/*.h
    Effekseer/Material/*.h
    Effekseer/Parameter/*.h
    Effekseer/IO/*.h
    Effekseer/Utils/*.h
    Effekseer/Noise/*.h)

set(effekseer_src
    Effekseer/Effekseer.Client.cpp
    Effekseer/Effekseer.Color.cpp
    Effekseer/Effekseer.DefaultEffectLoader.cpp
    Effekseer/Effekseer.DefaultFile.cpp
    Effekseer/Effekseer.Effect.cpp
    Effekseer/Effekseer.EffectNode.cpp
    Effekseer/Effekseer.EffectNodeModel.cpp
    Effekseer/Effekseer.EffectNodeRibbon.cpp
    Effekseer/Effekseer.EffectNodeRing.cpp
    Effekseer/Effekseer.EffectNodeRoot.cpp
    Effekseer/Effekseer.EffectNodeSprite.cpp
    Effekseer/Effekseer.EffectNodeTrack.cpp
    Effekseer/Effekseer.FCurves.cpp
    Effekseer/Effekseer.Instance.cpp
    Effekseer/Effekseer.InstanceChunk.cpp
    Effekseer/Effekseer.InstanceContainer.cpp
    Effekseer/Effekseer.InstanceGlobal.cpp
    Effekseer/Effekseer.InstanceGroup.cpp
    Effekseer/Effekseer.InternalScript.cpp
    Effekseer/Effekseer.Manager.cpp
    Effekseer/Effekseer.Matrix43.cpp
    Effekseer/Effekseer.Matrix44.cpp
    Effekseer/Effekseer.RectF.cpp
    Effekseer/Effekseer.Setting.cpp
    Effekseer/Effekseer.Server.cpp
    Effekseer/Effekseer.Socket.cpp
    Effekseer/Effekseer.Vector2D.cpp
    Effekseer/Effekseer.Vector3D.cpp
    Effekseer/Culling/Culling3D.Grid.cpp
    Effekseer/Culling/Culling3D.Layer.cpp
    Effekseer/Culling/Culling3D.Matrix44.cpp
    Effekseer/Culling/Culling3D.ObjectInternal.cpp
    Effekseer/Culling/Culling3D.ReferenceObject.cpp
    Effekseer/Culling/Culling3D.Vector3DF.cpp
    Effekseer/Culling/Culling3D.WorldInternal.cpp
    Effekseer/Material/Effekseer.Material.cpp
    Effekseer/Material/Effekseer.CompiledMaterial.cpp
    Effekseer/Material/Effekseer.MaterialCompiler.cpp
    Effekseer/IO/Effekseer.EfkEfcFactory.cpp
    Effekseer/Parameter/Effekseer.Parameters.cpp
    Effekseer/Utils/Effekseer.CustomAllocator.cpp
    Effekseer/SIMD/Effekseer.Mat43f.cpp
    Effekseer/SIMD/Effekseer.Mat44f.cpp
    Effekseer/SIMD/Effekseer.SIMDUtils.cpp
    Effekseer/Noise/CurlNoise.cpp
    Effekseer/Noise/PerlinNoise.cpp
)

add_library(${PROJECT_NAME} STATIC
    ${effekseer_h}
    ${effekseer_src})

set(files 
    ${effekseer_h} 
    ${effekseer_src})

FilterFolder("${files}")
if(CLANG_FORMAT_ENABLED)
    clang_format(Effekseer)
endif()

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /we4456 /we4458)
endif()

if(BUILD_UNITYPLUGIN)
    install(DIRECTORY "${PROJECT_SOURCE_DIR}"
        DESTINATION "include"
        FILES_MATCHING
        PATTERN "*.h"
    )
else()
    set(PublicHeader
        Effekseer.h 
        Effekseer.Internal.h)

    set_target_properties(
        ${PROJECT_NAME} PROPERTIES PUBLIC_HEADER 
        "${PublicHeader}")
endif()

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-export
    INCLUDES DESTINATION include
    PUBLIC_HEADER DESTINATION include/Effekseer
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib)

install(
    EXPORT ${PROJECT_NAME}-export
    FILE ${PROJECT_NAME}-config.cmake
    DESTINATION lib/cmake
    EXPORT_LINK_INTERFACE_LIBRARIES)