# Limit in Debug and Release
set(CMAKE_CONFIGURATION_TYPES Debug Release CACHE TYPE INTERNAL FORCE )

# The project's name
project(Effekseer)

# Check cmake version
cmake_minimum_required(VERSION 3.1)

# C++ version
if(WIN32)
    set(CMAKE_CXX_STANDARD 17)
else()
    set(CMAKE_CXX_STANDARD 14)
endif()

# For Mac
if(APPLE)
	set(CMAKE_OSX_DEPLOYMENT_TARGET 10.10)
endif()

option(USE_INTERNAL_LOADER "Use Internal Loader" ON)

option(USE_GLEW_STATIC "Use Glew static library" OFF)

if (WIN32)
option(USE_GLEW_DLL "Use Glew DLL" ON)
else()
option(USE_GLEW_DLL "Use Glew DLL" OFF)
endif()

option(USE_OPENGLES2 "Use OpenGL ES2" OFF)
option(USE_OPENGLES3 "Use OpenGL ES3" OFF)
option(USE_OPENGL3 "Use OpenGL 3" OFF)
option(USE_LLGI "Use LLGI" OFF)

if (MSVC)
option(USE_OPENAL "Use OpenAL" OFF)
else()
option(USE_OPENAL "Use OpenAL" ON)
endif()

if (WIN32)
option(USE_XAUDIO2 "Use XAudio2" OFF)
option(USE_DSOUND "Use DirectSound" OFF)
option(USE_OSM "Use OpenSoundMixer" OFF)
endif()

# Flags
if (MSVC)
	if (NOT USE_MSVC_RUNTIME_LIBRARY_DLL)
		foreach (flag CMAKE_C_FLAGS
			CMAKE_C_FLAGS_DEBUG
			CMAKE_C_FLAGS_RELEASE
			CMAKE_CXX_FLAGS
			CMAKE_CXX_FLAGS_DEBUG
			CMAKE_CXX_FLAGS_RELEASE)

			if (${flag} MATCHES "/MD")
				string(REGEX REPLACE "/MD" "/MT" ${flag} "${${flag}}")
			endif()
			if (${flag} MATCHES "/MDd")
				string(REGEX REPLACE "/MDd" "/MTd" ${flag} "${${flag}}")
			endif()
		endforeach()
	endif()
endif()

if (USE_INTERNAL_LOADER)
	add_definitions(-D__EFFEKSEER_RENDERER_INTERNAL_LOADER__)
endif()

if (USE_GLEW_STATIC)
	add_definitions(-D__EFFEKSEER_RENDERER_GL_GLEW_S__)
endif()

if (USE_GLEW_DLL)
	add_definitions(-D__EFFEKSEER_RENDERER_GL_GLEW__)
endif()

if (USE_OPENGLES2)
	add_definitions(-D__EFFEKSEER_RENDERER_GLES2__)
endif()

if (USE_OPENGLES3)
	add_definitions(-D__EFFEKSEER_RENDERER_GLES3__)
endif()

if (USE_OPENGL3)
	add_definitions(-D__EFFEKSEER_RENDERER_GL3__)
endif()

# Setup source codes
file(GLOB effekseer_h
    Effekseer/*.h
	Effekseer/Effekseer/*.h
	Effekseer/Effekseer/Renderer/*.h
	Effekseer/Effekseer/Culling/*.h
	Effekseer/Effekseer/Material/*.h
	Effekseer/Effekseer/Parameter/*.h
	Effekseer/Effekseer/IO/*.h
	Effekseer/Effekseer/Utils/*.h)

set(effekseer_src
	Effekseer/Effekseer/Effekseer.Client.cpp
	Effekseer/Effekseer/Effekseer.Color.cpp
	Effekseer/Effekseer/Effekseer.DefaultEffectLoader.cpp
	Effekseer/Effekseer/Effekseer.DefaultFile.cpp
	Effekseer/Effekseer/Effekseer.Effect.cpp
	Effekseer/Effekseer/Effekseer.EffectNode.cpp
	Effekseer/Effekseer/Effekseer.EffectNodeModel.cpp
	Effekseer/Effekseer/Effekseer.EffectNodeRibbon.cpp
	Effekseer/Effekseer/Effekseer.EffectNodeRing.cpp
	Effekseer/Effekseer/Effekseer.EffectNodeRoot.cpp
	Effekseer/Effekseer/Effekseer.EffectNodeSprite.cpp
	Effekseer/Effekseer/Effekseer.EffectNodeTrack.cpp
	Effekseer/Effekseer/Effekseer.FCurves.cpp
	Effekseer/Effekseer/Effekseer.Instance.cpp
	Effekseer/Effekseer/Effekseer.InstanceChunk.cpp
	Effekseer/Effekseer/Effekseer.InstanceContainer.cpp
	Effekseer/Effekseer/Effekseer.InstanceGlobal.cpp
	Effekseer/Effekseer/Effekseer.InstanceGroup.cpp
	Effekseer/Effekseer/Effekseer.InternalScript.cpp
	Effekseer/Effekseer/Effekseer.Manager.cpp
	Effekseer/Effekseer/Effekseer.Matrix43.cpp
	Effekseer/Effekseer/Effekseer.Matrix44.cpp
	Effekseer/Effekseer/Effekseer.RectF.cpp
	Effekseer/Effekseer/Effekseer.Setting.cpp
	Effekseer/Effekseer/Effekseer.Server.cpp
	Effekseer/Effekseer/Effekseer.Socket.cpp
	Effekseer/Effekseer/Effekseer.Vector2D.cpp
	Effekseer/Effekseer/Effekseer.Vector3D.cpp
	Effekseer/Effekseer/Culling/Culling3D.Grid.cpp
	Effekseer/Effekseer/Culling/Culling3D.Layer.cpp
	Effekseer/Effekseer/Culling/Culling3D.Matrix44.cpp
	Effekseer/Effekseer/Culling/Culling3D.ObjectInternal.cpp
	Effekseer/Effekseer/Culling/Culling3D.ReferenceObject.cpp
	Effekseer/Effekseer/Culling/Culling3D.Vector3DF.cpp
	Effekseer/Effekseer/Culling/Culling3D.WorldInternal.cpp
	Effekseer/Effekseer/Material/Effekseer.Material.cpp
	Effekseer/Effekseer/Material/Effekseer.CompiledMaterial.cpp
	Effekseer/Effekseer/Material/Effekseer.MaterialCompiler.cpp
	Effekseer/Effekseer/IO/Effekseer.EfkEfcFactory.cpp
	Effekseer/Effekseer/Parameter/Effekseer.Parameters.cpp
	Effekseer/Effekseer/Utils/Effekseer.CustomAllocator.cpp
)

file(GLOB effekseer_renderer_common_h
    EffekseerRendererCommon/*.h)

set(effekseer_renderer_common_src
	EffekseerRendererCommon/EffekseerRenderer.CommonUtils.cpp
	EffekseerRendererCommon/EffekseerRenderer.IndexBufferBase.cpp
	EffekseerRendererCommon/EffekseerRenderer.ModelRendererBase.cpp
	EffekseerRendererCommon/EffekseerRenderer.RenderStateBase.cpp
	EffekseerRendererCommon/EffekseerRenderer.Renderer.cpp
	EffekseerRendererCommon/EffekseerRenderer.Renderer_Impl.cpp
	EffekseerRendererCommon/EffekseerRenderer.RibbonRendererBase.cpp
	EffekseerRendererCommon/EffekseerRenderer.RingRendererBase.cpp
	EffekseerRendererCommon/EffekseerRenderer.SpriteRendererBase.cpp
	EffekseerRendererCommon/EffekseerRenderer.TrackRendererBase.cpp
	EffekseerRendererCommon/EffekseerRenderer.VertexBufferBase.cpp
	EffekseerRendererCommon/EffekseerRenderer.DDSTextureLoader.cpp)

file(GLOB_RECURSE effekseer_renderer_dx9_h
    EffekseerRendererDX9/*.h)

set(effekseer_renderer_dx9_src
	EffekseerRendererDX9/EffekseerRenderer/EffekseerRendererDX9.DeviceObject.cpp
	EffekseerRendererDX9/EffekseerRenderer/EffekseerRendererDX9.IndexBuffer.cpp
	EffekseerRendererDX9/EffekseerRenderer/EffekseerRendererDX9.ModelLoader.cpp
	EffekseerRendererDX9/EffekseerRenderer/EffekseerRendererDX9.MaterialLoader.cpp
	EffekseerRendererDX9/EffekseerRenderer/EffekseerRendererDX9.ModelRenderer.cpp
	EffekseerRendererDX9/EffekseerRenderer/EffekseerRendererDX9.Renderer.cpp
	EffekseerRendererDX9/EffekseerRenderer/EffekseerRendererDX9.RenderState.cpp
	EffekseerRendererDX9/EffekseerRenderer/EffekseerRendererDX9.Shader.cpp
	EffekseerRendererDX9/EffekseerRenderer/EffekseerRendererDX9.TextureLoader.cpp
	EffekseerRendererDX9/EffekseerRenderer/EffekseerRendererDX9.VertexBuffer.cpp)

file(GLOB_RECURSE effekseer_renderer_dx11_h
    EffekseerRendererDX11/*.h)

set(effekseer_renderer_dx11_src
	EffekseerRendererDX11/EffekseerRenderer/EffekseerRendererDX11.DeviceObject.cpp
	EffekseerRendererDX11/EffekseerRenderer/EffekseerRendererDX11.IndexBuffer.cpp
	EffekseerRendererDX11/EffekseerRenderer/EffekseerRendererDX11.ModelLoader.cpp
	EffekseerRendererDX11/EffekseerRenderer/EffekseerRendererDX11.ModelRenderer.cpp
	EffekseerRendererDX11/EffekseerRenderer/EffekseerRendererDX11.MaterialLoader.cpp
	EffekseerRendererDX11/EffekseerRenderer/EffekseerRendererDX11.Renderer.cpp
	EffekseerRendererDX11/EffekseerRenderer/EffekseerRendererDX11.RenderState.cpp
	EffekseerRendererDX11/EffekseerRenderer/EffekseerRendererDX11.Shader.cpp
	EffekseerRendererDX11/EffekseerRenderer/EffekseerRendererDX11.TextureLoader.cpp
	EffekseerRendererDX11/EffekseerRenderer/EffekseerRendererDX11.VertexBuffer.cpp
	
	EffekseerMaterialCompiler/DirectX11/EffekseerMaterialCompilerDX11.cpp
	EffekseerMaterialCompiler/DirectX11/EffekseerMaterialCompilerDX11.h
	)

file(GLOB_RECURSE effekseer_renderer_gl_h
    EffekseerRendererGL/*.h)

set(effekseer_renderer_gl_src
	EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.DeviceObject.cpp
	EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.GLExtension.cpp
	EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.IndexBuffer.cpp
	EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.ModelLoader.cpp
	EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.ModelRenderer.cpp
	EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.MaterialLoader.cpp
	EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.Renderer.cpp
	EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.RenderState.cpp
	EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.Shader.cpp
	EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.TextureLoader.cpp
	EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.VertexArray.cpp
	EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.VertexBuffer.cpp
	EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.VertexBuffer.cpp
	EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.DeviceObjectCollection.cpp

	EffekseerMaterialCompiler/OpenGL/EffekseerMaterialCompilerGL.cpp
	EffekseerMaterialCompiler/OpenGL/EffekseerMaterialCompilerGL.h
	)

list(APPEND effekseer_renderer_dx9_src
	${effekseer_renderer_dx9_h}
    ${effekseer_renderer_common_h}
	${effekseer_renderer_common_src})

list(APPEND effekseer_renderer_dx11_src
	${effekseer_renderer_dx11_h}
    ${effekseer_renderer_common_h}
	${effekseer_renderer_common_src})

list(APPEND effekseer_renderer_gl_src
	${effekseer_renderer_gl_h}
    ${effekseer_renderer_common_h}
	${effekseer_renderer_common_src})

if (USE_INTERNAL_LOADER)
set(effekseer_renderer_dx_loader_src
	EffekseerRendererCommon/EffekseerRenderer.DXTK.DDSTextureLoader.cpp
	EffekseerRendererCommon/EffekseerRenderer.PngTextureLoader.cpp)

set(effekseer_renderer_gl_loader_src
	EffekseerRendererCommon/EffekseerRenderer.PngTextureLoader.cpp)

list(APPEND effekseer_renderer_dx9_src
	${effekseer_renderer_dx_loader_src})

list(APPEND effekseer_renderer_dx11_src
	${effekseer_renderer_dx_loader_src})

list(APPEND effekseer_renderer_gl_src
	${effekseer_renderer_gl_loader_src})

endif()

# set include directories
include_directories(
	Effekseer/
	include/
	)

# set lib directories

link_directories(
	${PROJECT_SOURCE_DIR}/lib/
	)

# Add compile targets
add_library(Effekseer STATIC
    ${effekseer_h}
    ${effekseer_src} )

if (BUILD_DX12)
    set(BUILD_TEST_TEMP ${BUILD_TEST})
    set(BUILD_TEST OFF)
    add_subdirectory(3rdParty/LLGI)
    set(BUILD_TEST ${BUILD_TEST_TEMP})
endif()

if (BUILD_DX12)
    add_subdirectory(EffekseerRendererDX12)
endif()
if (BUILD_VULKAN)
    add_subdirectory(EffekseerRendererVulkan)
endif()

if (WIN32)
    add_library(EffekseerRendererDX9 STATIC ${effekseer_renderer_dx9_src} )
    add_library(EffekseerRendererDX11 STATIC ${effekseer_renderer_dx11_src} )
endif()

add_library(EffekseerRendererGL STATIC ${effekseer_renderer_gl_src} )

if (USE_OPENAL)
	add_subdirectory(EffekseerSoundAL)
endif()

if (USE_DSOUND)
	add_subdirectory(EffekseerSoundDSound)
endif()

if (USE_XAUDIO2)
	add_subdirectory(EffekseerSoundXAudio2)
endif()

if (USE_OSM OR BUILD_VIEWER)
	add_subdirectory("EffekseerSoundOSMixer")
endif()

# filter
set(files 
    ${effekseer_h} 
	${effekseer_src} 
	${effekseer_renderer_common_h} 
	${effekseer_renderer_src} 
	${effekseer_renderer_dx9_src} 
	${effekseer_renderer_dx11_src} 
	${effekseer_renderer_gl_src})

FilterFolder("${files}")

install(
	TARGETS Effekseer
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib)

if (MSVC)
install(
	TARGETS EffekseerRendererDX9
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib)
endif()

if (MSVC)
install(
	TARGETS EffekseerRendererDX11
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib)
endif()

install(
	TARGETS EffekseerRendererGL
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib)

if (BUILD_VIEWER)
    add_subdirectory("EditorCommon")
    add_subdirectory("Viewer")
    add_subdirectory("IPC")
    add_subdirectory("EffekseerMaterialCompiler")
    add_dependencies(Viewer ResourceData)
endif()


if (BUILD_TEST)
	add_subdirectory("Test")
endif()

if(WIN32 AND BUILD_VIEWER)

    set(BUILD_TEST_TEMP ${BUILD_TEST})
    set(BUILD_TEST OFF)
	add_subdirectory("3rdParty/AltseedRHI")
    set(BUILD_TEST ${BUILD_TEST_TEMP})

	add_dependencies(AltseedRHI ExternalProject_glew)
	target_include_directories(
	    AltseedRHI
		PRIVATE
		${EFK_THIRDPARTY_INCLUDES}
	)

	add_subdirectory("EffekseerMaterial")
	add_subdirectory("EffekseerMaterialEditor")
endif()