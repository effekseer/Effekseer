project(Effekseer)
cmake_minimum_required(VERSION 3.15)

FIND_PACKAGE(Threads REQUIRED)

# Setup source codes
set(effekseer_public_h
    Effekseer.h
    Effekseer.Modules.h
    Effekseer.SIMD.h
    Effekseer/Effekseer.Base.Pre.h
    Effekseer/Effekseer.Effect.h
    Effekseer/Effekseer.Color.h
    Effekseer/Effekseer.DefaultFile.h
    Effekseer/Effekseer.File.h
    Effekseer/Effekseer.Manager.h
    Effekseer/Effekseer.Matrix43.h
    Effekseer/Effekseer.Matrix44.h
    Effekseer/Effekseer.Resource.h
    Effekseer/Effekseer.RectF.h
    Effekseer/Effekseer.Setting.h
    Effekseer/Effekseer.Vector2D.h
    Effekseer/Effekseer.Vector3D.h
    Effekseer/Backend/GraphicsDevice.h
    Effekseer/Network/Effekseer.Server.h
    Effekseer/Network/Effekseer.Client.h
    Effekseer/Utils/Effekseer.CustomAllocator.h
    Effekseer/IO/Effekseer.EfkEfcFactory.h

    Effekseer/SIMD/Base.h
    Effekseer/SIMD/Float4_Gen.h
    Effekseer/SIMD/Float4_NEON.h
    Effekseer/SIMD/Float4_SSE.h
    Effekseer/SIMD/Int4_Gen.h
    Effekseer/SIMD/Int4_NEON.h
    Effekseer/SIMD/Int4_SSE.h
    Effekseer/SIMD/Bridge_Gen.h
    Effekseer/SIMD/Bridge_NEON.h
    Effekseer/SIMD/Bridge_SSE.h
    Effekseer/SIMD/Vec2f.h
    Effekseer/SIMD/Vec3f.h
    Effekseer/SIMD/Vec4f.h
    Effekseer/SIMD/Mat43f.h
    Effekseer/SIMD/Mat44f.h
    Effekseer/SIMD/Quaternionf.h
    Effekseer/SIMD/Utils.h

    Effekseer/Parameter/Effekseer.Parameters.h
    Effekseer/Renderer/Effekseer.SpriteRenderer.h
    Effekseer/Renderer/Effekseer.RibbonRenderer.h
    Effekseer/Renderer/Effekseer.RingRenderer.h
    Effekseer/Renderer/Effekseer.ModelRenderer.h
    Effekseer/Renderer/Effekseer.TrackRenderer.h
    Effekseer/Renderer/Effekseer.GPUTimer.h
    Effekseer/Effekseer.EffectLoader.h
    Effekseer/Effekseer.TextureLoader.h
    Effekseer/Model/Model.h
    Effekseer/Model/ModelLoader.h
    Effekseer/Effekseer.MaterialLoader.h
    Effekseer/Model/Model.h
    Effekseer/Effekseer.Curve.h
    Effekseer/Effekseer.CurveLoader.h
    Effekseer/Sound/Effekseer.SoundPlayer.h
    Effekseer/Effekseer.SoundLoader.h
    Effekseer/Effekseer.WorkerThread.h
    Effekseer/Effekseer.ResourceManager.h
)

file(GLOB effekseer_h
    *.h
    Effekseer/*.h
    Effekseer/Renderer/*.h
    Effekseer/Material/*.h
    Effekseer/Parameter/*.h
    Effekseer/Parameter/AllTypeColor.h
    Effekseer/Parameter/AlphaCutoff.h
    Effekseer/Parameter/BasicSettings.h
    Effekseer/Parameter/CustomData.h
    Effekseer/Parameter/DepthParameter.h
    Effekseer/Parameter/KillRules.h
    Effekseer/Parameter/LOD.h
    Effekseer/Parameter/Rotation.h
    Effekseer/Parameter/Scaling.h
    Effekseer/Parameter/Sound.h
    Effekseer/Parameter/UV.h
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
    Effekseer/Network/*.h
    Effekseer/Network/data/*.h
)

set(effekseer_src
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
    Effekseer/Effekseer.Vector2D.cpp
    Effekseer/Effekseer.Vector3D.cpp
    Effekseer/Effekseer.WorkerThread.cpp
    Effekseer/Material/Effekseer.MaterialFile.cpp
    Effekseer/Material/Effekseer.CompiledMaterial.cpp
    Effekseer/Material/Effekseer.MaterialCompiler.cpp
    Effekseer/IO/Effekseer.EfkEfcFactory.cpp
    Effekseer/Parameter/Easing.cpp
    Effekseer/Parameter/Effekseer.Parameters.cpp
    Effekseer/Parameter/Rotation.cpp
    Effekseer/Utils/Effekseer.CustomAllocator.cpp
    Effekseer/SIMD/Mat43f.cpp
    Effekseer/SIMD/Mat44f.cpp
    Effekseer/SIMD/Utils.cpp
    Effekseer/Noise/CurlNoise.cpp
    Effekseer/ForceField/ForceFields.cpp
    Effekseer/Model/ProceduralModelGenerator.cpp
    Effekseer/Model/Model.cpp
    Effekseer/Model/ModelLoader.cpp
    Effekseer/Model/SplineGenerator.cpp
    Effekseer/Network/Effekseer.Client.cpp
    Effekseer/Network/Effekseer.Server.cpp
    Effekseer/Network/Effekseer.Session.cpp
    Effekseer/Network/Effekseer.Socket.cpp
)

add_library(${PROJECT_NAME} STATIC
    ${effekseer_public_h}
    ${effekseer_h}
    ${effekseer_src})

set(files
    ${effekseer_public_h}
    ${effekseer_h}
    ${effekseer_src})

target_include_directories(Effekseer PRIVATE ${EFK_THIRDPARTY_INCLUDES})
target_link_libraries(Effekseer PUBLIC ${CMAKE_THREAD_LIBS_INIT})

FilterFolder("${files}")
if(CLANG_FORMAT_ENABLED)
    clang_format(Effekseer)
endif()

if(NETWORK_ENABLED)
    target_compile_definitions(Effekseer PUBLIC __EFFEKSEER_NETWORK_ENABLED__)
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

WarnError(${PROJECT_NAME})

foreach (file ${effekseer_public_h})
    get_filename_component(dir ${file} DIRECTORY)
    install(FILES ${file} DESTINATION include/Effekseer/${dir})
endforeach()

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-export
    INCLUDES DESTINATION include
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib)

install(
    EXPORT ${PROJECT_NAME}-export
    FILE ${PROJECT_NAME}-config.cmake
    DESTINATION lib/cmake
    EXPORT_LINK_INTERFACE_LIBRARIES)
