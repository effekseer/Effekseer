# Debug Releaseに限定
set(CMAKE_CONFIGURATION_TYPES Debug Release CACHE TYPE INTERNAL FORCE )

# プロジェクト名
project(Effekseer)

# CMake バージョンチェック
cmake_minimum_required(VERSION 2.8)

# MSVC向け設定
if (MSVC)
	option(USE_MSVC_RUNTIME_LIBRARY_DLL "Use MSVC runtime library DLL" OFF)
	option(USE_DIRECTXTEX "Use DirectXTex" OFF)
endif()

option(USE_GLEW_STATIC "Use Glew static library" ON)
option(USE_GLEW_DLL "Use Glew DLL" OFF)
option(USE_OPENGLES2 "Use OpenGL ES2" OFF)

# ソースコード設定
set(effekseer_src
	Effekseer/Effekseer/Effekseer.Client.cpp
	Effekseer/Effekseer/Effekseer.Color.cpp
	Effekseer/Effekseer/Effekseer.CriticalSection.cpp
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
	Effekseer/Effekseer/Effekseer.InstanceContainer.cpp
	Effekseer/Effekseer/Effekseer.InstanceGlobal.cpp
	Effekseer/Effekseer/Effekseer.InstanceGroup.cpp
	Effekseer/Effekseer/Effekseer.Manager.cpp
	Effekseer/Effekseer/Effekseer.Matrix43.cpp
	Effekseer/Effekseer/Effekseer.Matrix44.cpp
	Effekseer/Effekseer/Effekseer.RectF.cpp
	Effekseer/Effekseer/Effekseer.Server.cpp
	Effekseer/Effekseer/Effekseer.Socket.cpp
	Effekseer/Effekseer/Effekseer.Thread.cpp
	Effekseer/Effekseer/Effekseer.Vector2D.cpp
	Effekseer/Effekseer/Effekseer.Vector3D.cpp)


set(effekseer_renderer_common_src
	EffekseerRendererCommon/EffekseerRenderer.IndexBufferBase.cpp
	EffekseerRendererCommon/EffekseerRenderer.ModelRendererBase.cpp
	EffekseerRendererCommon/EffekseerRenderer.RenderStateBase.cpp
	EffekseerRendererCommon/EffekseerRenderer.RibbonRendererBase.cpp
	EffekseerRendererCommon/EffekseerRenderer.RingRendererBase.cpp
	EffekseerRendererCommon/EffekseerRenderer.SpriteRendererBase.cpp
	EffekseerRendererCommon/EffekseerRenderer.TrackRendererBase.cpp
	EffekseerRendererCommon/EffekseerRenderer.VertexBufferBase.cpp)

set(effekseer_renderer_dx9_src
	EffekseerRendererDX9/EffekseerRenderer/EffekseerRenderer.DeviceObject.cpp
	EffekseerRendererDX9/EffekseerRenderer/EffekseerRenderer.IndexBuffer.cpp
	EffekseerRendererDX9/EffekseerRenderer/EffekseerRenderer.ModelLoader.cpp
	EffekseerRendererDX9/EffekseerRenderer/EffekseerRenderer.ModelRenderer.cpp
	EffekseerRendererDX9/EffekseerRenderer/EffekseerRenderer.Renderer.cpp
	EffekseerRendererDX9/EffekseerRenderer/EffekseerRenderer.RenderState.cpp
	EffekseerRendererDX9/EffekseerRenderer/EffekseerRenderer.RibbonRenderer.cpp
	EffekseerRendererDX9/EffekseerRenderer/EffekseerRenderer.RingRenderer.cpp
	EffekseerRendererDX9/EffekseerRenderer/EffekseerRenderer.Shader.cpp
	EffekseerRendererDX9/EffekseerRenderer/EffekseerRenderer.SpriteRenderer.cpp
	EffekseerRendererDX9/EffekseerRenderer/EffekseerRenderer.TextureLoader.cpp
	EffekseerRendererDX9/EffekseerRenderer/EffekseerRenderer.TrackRenderer.cpp
	EffekseerRendererDX9/EffekseerRenderer/EffekseerRenderer.VertexBuffer.cpp)

set(effekseer_renderer_dx11_src
	EffekseerRendererDX11/EffekseerRenderer/EffekseerRenderer.DeviceObject.cpp
	EffekseerRendererDX11/EffekseerRenderer/EffekseerRenderer.IndexBuffer.cpp
	EffekseerRendererDX11/EffekseerRenderer/EffekseerRenderer.ModelLoader.cpp
	EffekseerRendererDX11/EffekseerRenderer/EffekseerRenderer.ModelRenderer.cpp
	EffekseerRendererDX11/EffekseerRenderer/EffekseerRenderer.Renderer.cpp
	EffekseerRendererDX11/EffekseerRenderer/EffekseerRenderer.RenderState.cpp
	EffekseerRendererDX11/EffekseerRenderer/EffekseerRenderer.RibbonRenderer.cpp
	EffekseerRendererDX11/EffekseerRenderer/EffekseerRenderer.RingRenderer.cpp
	EffekseerRendererDX11/EffekseerRenderer/EffekseerRenderer.Shader.cpp
	EffekseerRendererDX11/EffekseerRenderer/EffekseerRenderer.SpriteRenderer.cpp
	EffekseerRendererDX11/EffekseerRenderer/EffekseerRenderer.TextureLoader.cpp
	EffekseerRendererDX11/EffekseerRenderer/EffekseerRenderer.TrackRenderer.cpp
	EffekseerRendererDX11/EffekseerRenderer/EffekseerRenderer.VertexBuffer.cpp)

set(effekseer_renderer_gl_src
	EffekseerRendererGL/EffekseerRenderer/EffekseerRenderer.DeviceObject.cpp
	EffekseerRendererGL/EffekseerRenderer/EffekseerRenderer.IndexBuffer.cpp
	EffekseerRendererGL/EffekseerRenderer/EffekseerRenderer.ModelLoader.cpp
	EffekseerRendererGL/EffekseerRenderer/EffekseerRenderer.ModelRenderer.cpp
	EffekseerRendererGL/EffekseerRenderer/EffekseerRenderer.Renderer.cpp
	EffekseerRendererGL/EffekseerRenderer/EffekseerRenderer.RenderState.cpp
	EffekseerRendererGL/EffekseerRenderer/EffekseerRenderer.RibbonRenderer.cpp
	EffekseerRendererGL/EffekseerRenderer/EffekseerRenderer.RingRenderer.cpp
	EffekseerRendererGL/EffekseerRenderer/EffekseerRenderer.Shader.cpp
	EffekseerRendererGL/EffekseerRenderer/EffekseerRenderer.SpriteRenderer.cpp
	EffekseerRendererGL/EffekseerRenderer/EffekseerRenderer.TextureLoader.cpp
	EffekseerRendererGL/EffekseerRenderer/EffekseerRenderer.TrackRenderer.cpp
	EffekseerRendererGL/EffekseerRenderer/EffekseerRenderer.VertexBuffer.cpp)

list(APPEND effekseer_renderer_dx9_src
	${effekseer_renderer_common_src})

list(APPEND effekseer_renderer_dx11_src
	${effekseer_renderer_common_src})

list(APPEND effekseer_renderer_gl_src
	${effekseer_renderer_common_src})

# インクルードの設定
include_directories(
	include/
	)

# コンパイル先を追加
add_library( effekseer STATIC ${effekseer_src} )

if (MSVC)
add_library( effekseer_renderer_dx9 STATIC ${effekseer_renderer_dx9_src} )
add_library( effekseer_renderer_dx11 STATIC ${effekseer_renderer_dx11_src} )
endif()

add_library( effekseer_renderer_gl STATIC ${effekseer_renderer_gl_src} )

# フラグ処理
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

if (USE_GLEW_STATIC)
	add_definitions(-D__EFFEKSEER_RENDERER_GL_GLEW_S__)
endif()

if (USE_GLEW_DLL)
	add_definitions(-D__EFFEKSEER_RENDERER_GL_GLEW__)
endif()

if (USE_OPENGLES2)
	add_definitions(-D__EFFEKSEER_RENDERER_GLES2__)
endif()

if (MSVC)
if (USE_DIRECTXTEX)
	add_definitions(-D__EFFEKSEER_RENDERER_DIRECTXTEX)
endif()
endif()


# 出力先の設定
if (MSVC)
set_target_properties(effekseer PROPERTIES OUTPUT_NAME_DEBUG "Effekseer" )
set_target_properties(effekseer PROPERTIES OUTPUT_NAME_RELEASE "Effekseer" )
else()
set_target_properties(effekseer PROPERTIES OUTPUT_NAME "Effekseer" )
endif()

if (MSVC)
if (MSVC)
set_target_properties(effekseer_renderer_dx9 PROPERTIES OUTPUT_NAME_DEBUG "EffekseerRendererDX9" )
set_target_properties(effekseer_renderer_dx9 PROPERTIES OUTPUT_NAME_RELEASE "EffekseerRendererDX9" )
else()
set_target_properties(effekseer_renderer_dx9 PROPERTIES OUTPUT_NAME "EffekseerRendererDX9" )
endif()
endif()

if (MSVC)
if (MSVC)
set_target_properties(effekseer_renderer_dx11 PROPERTIES OUTPUT_NAME_DEBUG "EffekseerRendererDX11" )
set_target_properties(effekseer_renderer_dx11 PROPERTIES OUTPUT_NAME_RELEASE "EffekseerRendererDX11" )
else()
set_target_properties(effekseer_renderer_dx11 PROPERTIES OUTPUT_NAME "EffekseerRendererDX11" )
endif()
endif()

if (MSVC)
set_target_properties(effekseer_renderer_gl PROPERTIES OUTPUT_NAME_DEBUG "EffekseerRendererGL" )
set_target_properties(effekseer_renderer_gl PROPERTIES OUTPUT_NAME_RELEASE "EffekseerRendererGL" )
else()
set_target_properties(effekseer_renderer_gl PROPERTIES OUTPUT_NAME "EffekseerRendererGL" )
endif()


install(
	TARGETS effekseer
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib)

if (MSVC)
install(
	TARGETS effekseer_renderer_dx9
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib)
endif()

if (MSVC)
install(
	TARGETS effekseer_renderer_dx11
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib)
endif()

install(
	TARGETS effekseer_renderer_gl
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib)