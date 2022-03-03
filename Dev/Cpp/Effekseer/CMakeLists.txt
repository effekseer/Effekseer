project(Effekseer)
cmake_minimum_required(VERSION 3.1)

FIND_PACKAGE(Threads REQUIRED)

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
    Effekseer/Noise/*.h
    Effekseer/ForceField/*.h
    Effekseer/Backend/*.h
    Effekseer/Model/ProceduralModelGenerator.h
    Effekseer/Model/ProceduralModelParameter.h
    Effekseer/Model/Model.h
    Effekseer/Model/ModelLoader.h
    Effekseer/Model/SplineGenerator.h
)

set(effekseer_src
    Effekseer/Effekseer.Client.cpp
    Effekseer/Effekseer.Color.cpp
    Effekseer/Effekseer.CurveLoader.cpp
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
    Effekseer/Effekseer.Random.cpp
    Effekseer/Effekseer.RectF.cpp
    Effekseer/Effekseer.Resource.cpp
    Effekseer/Effekseer.ResourceManager.cpp
    Effekseer/Effekseer.Setting.cpp
    Effekseer/Effekseer.Server.cpp
    Effekseer/Effekseer.Socket.cpp
    Effekseer/Effekseer.Vector2D.cpp
    Effekseer/Effekseer.Vector3D.cpp
    Effekseer/Effekseer.WorkerThread.cpp
    Effekseer/Culling/Culling3D.Grid.cpp
    Effekseer/Culling/Culling3D.Layer.cpp
    Effekseer/Culling/Culling3D.Matrix44.cpp
    Effekseer/Culling/Culling3D.ObjectInternal.cpp
    Effekseer/Culling/Culling3D.ReferenceObject.cpp
    Effekseer/Culling/Culling3D.Vector3DF.cpp
    Effekseer/Culling/Culling3D.WorldInternal.cpp
    Effekseer/Material/Effekseer.MaterialFile.cpp
    Effekseer/Material/Effekseer.CompiledMaterial.cpp
    Effekseer/Material/Effekseer.MaterialCompiler.cpp
    Effekseer/IO/Effekseer.EfkEfcFactory.cpp
    Effekseer/Parameter/Easing.cpp
    Effekseer/Parameter/Effekseer.Parameters.cpp
    Effekseer/Utils/Effekseer.CustomAllocator.cpp
    Effekseer/SIMD/Mat43f.cpp
    Effekseer/SIMD/Mat44f.cpp
    Effekseer/SIMD/Utils.cpp
    Effekseer/Noise/CurlNoise.cpp
    Effekseer/Noise/PerlinNoise.cpp
    Effekseer/ForceField/ForceFields.cpp
    Effekseer/Model/ProceduralModelGenerator.cpp
    Effekseer/Model/Model.cpp
    Effekseer/Model/ModelLoader.cpp
    Effekseer/Model/SplineGenerator.cpp
)

add_library(${PROJECT_NAME} STATIC
    ${effekseer_h}
    ${effekseer_src})

set(files
    ${effekseer_h}
    ${effekseer_src})

target_link_libraries(Effekseer PUBLIC ${CMAKE_THREAD_LIBS_INIT})

FilterFolder("${files}")
if(CLANG_FORMAT_ENABLED)
    clang_format(Effekseer)
endif()

if(BUILD_WITH_EASY_PROFILER)
    target_compile_definitions(Effekseer PUBLIC BUILD_WITH_EASY_PROFILER)
    target_include_directories(Effekseer PUBLIC $<BUILD_INTERFACE:${EFK_PROFILER_INCLUDES}>)
    target_link_directories(Effekseer PUBLIC $<BUILD_INTERFACE:${EFK_PROFILER_LIBRARY_DIRECTORIES}>)

    target_link_libraries(Effekseer PUBLIC 
        debug ${EASY_PROFILER_LIB_DEBUG}
        optimized ${EASY_PROFILER_LIB_RELEASE})

    if(MSVC)
        target_link_libraries(Effekseer PUBLIC psapi)
    endif()

    add_dependencies(Effekseer ExternalProject_easy_profiler)
endif()

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /we4456 /we4458)
endif()

if(BUILD_UNITYPLUGIN OR BUILD_UNITYPLUGIN_FOR_IOS)
    install(DIRECTORY "${PROJECT_SOURCE_DIR}"
        DESTINATION "include"
        FILES_MATCHING
        PATTERN "*.h"
    )
else()
    set(PublicHeader
        Effekseer.h
        Effekseer.Modules.h
        Effekseer.SIMD.h)

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
