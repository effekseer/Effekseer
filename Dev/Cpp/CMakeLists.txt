# Debug Releaseに限定
set(CMAKE_CONFIGURATION_TYPES Debug Release CACHE TYPE INTERNAL FORCE )

# プロジェクト名
project(Effekseer)

# CMake バージョンチェック
cmake_minimum_required(VERSION 2.8)

# MSVC向け設定
if (MSVC)
	option(USE_MSVC_RUNTIME_LIBRARY_DLL "Use MSVC runtime library DLL" OFF)
endif()

if (MSVC)

elseif (MINGW)
	set(CMAKE_CXX_FLAGS "-std=c++11" ${CMAKE_CXX_FLAGS})
else()
	set(CMAKE_CXX_FLAGS "-std=c++11 -fPIC" ${CMAKE_CXX_FLAGS})
endif()

option(USE_INTERNAL_LOADER "Use Internal Loader" ON)

option(USE_GLEW_STATIC "Use Glew static library" OFF)
option(USE_GLEW_DLL "Use Glew DLL" ON)

option(USE_OPENGLES2 "Use OpenGL ES2" OFF)
option(USE_OPENGLES3 "Use OpenGL ES3" OFF)
option(USE_OPENGL3 "Use OpenGL 3" OFF)

if (MSVC)
option(USE_OPENAL "Use OpenAL" OFF)
else()
option(USE_OPENAL "Use OpenAL" ON)
endif()

if (WIN32)
option(USE_XAUDIO2 "Use XAudio2" ON)
endif()

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
	Effekseer/Effekseer/Effekseer.Setting.cpp
	Effekseer/Effekseer/Effekseer.Server.cpp
	Effekseer/Effekseer/Effekseer.Socket.cpp
	Effekseer/Effekseer/Effekseer.Thread.cpp
	Effekseer/Effekseer/Effekseer.Vector2D.cpp
	Effekseer/Effekseer/Effekseer.Vector3D.cpp
	Effekseer/Effekseer/Culling/Culling3D.Grid.cpp
	Effekseer/Effekseer/Culling/Culling3D.Layer.cpp
	Effekseer/Effekseer/Culling/Culling3D.Matrix44.cpp
	Effekseer/Effekseer/Culling/Culling3D.ObjectInternal.cpp
	Effekseer/Effekseer/Culling/Culling3D.ReferenceObject.cpp
	Effekseer/Effekseer/Culling/Culling3D.Vector3DF.cpp
	Effekseer/Effekseer/Culling/Culling3D.WorldInternal.cpp)

set(effekseer_renderer_common_src
	EffekseerRendererCommon/EffekseerRenderer.IndexBufferBase.cpp
	EffekseerRendererCommon/EffekseerRenderer.ModelRendererBase.cpp
	EffekseerRendererCommon/EffekseerRenderer.RenderStateBase.cpp
	EffekseerRendererCommon/EffekseerRenderer.Renderer.cpp
	EffekseerRendererCommon/EffekseerRenderer.RibbonRendererBase.cpp
	EffekseerRendererCommon/EffekseerRenderer.RingRendererBase.cpp
	EffekseerRendererCommon/EffekseerRenderer.SpriteRendererBase.cpp
	EffekseerRendererCommon/EffekseerRenderer.TrackRendererBase.cpp
	EffekseerRendererCommon/EffekseerRenderer.VertexBufferBase.cpp
	EffekseerRendererCommon/EffekseerRenderer.DDSTextureLoader.cpp)

set(effekseer_renderer_dx9_src
	EffekseerRendererDX9/EffekseerRenderer/EffekseerRendererDX9.DeviceObject.cpp
	EffekseerRendererDX9/EffekseerRenderer/EffekseerRendererDX9.IndexBuffer.cpp
	EffekseerRendererDX9/EffekseerRenderer/EffekseerRendererDX9.ModelLoader.cpp
	EffekseerRendererDX9/EffekseerRenderer/EffekseerRendererDX9.ModelRenderer.cpp
	EffekseerRendererDX9/EffekseerRenderer/EffekseerRendererDX9.Renderer.cpp
	EffekseerRendererDX9/EffekseerRenderer/EffekseerRendererDX9.RenderState.cpp
	EffekseerRendererDX9/EffekseerRenderer/EffekseerRendererDX9.Shader.cpp
	EffekseerRendererDX9/EffekseerRenderer/EffekseerRendererDX9.TextureLoader.cpp
	EffekseerRendererDX9/EffekseerRenderer/EffekseerRendererDX9.VertexBuffer.cpp)

set(effekseer_renderer_dx11_src
	EffekseerRendererDX11/EffekseerRenderer/EffekseerRendererDX11.DeviceObject.cpp
	EffekseerRendererDX11/EffekseerRenderer/EffekseerRendererDX11.IndexBuffer.cpp
	EffekseerRendererDX11/EffekseerRenderer/EffekseerRendererDX11.ModelLoader.cpp
	EffekseerRendererDX11/EffekseerRenderer/EffekseerRendererDX11.ModelRenderer.cpp
	EffekseerRendererDX11/EffekseerRenderer/EffekseerRendererDX11.Renderer.cpp
	EffekseerRendererDX11/EffekseerRenderer/EffekseerRendererDX11.RenderState.cpp
	EffekseerRendererDX11/EffekseerRenderer/EffekseerRendererDX11.Shader.cpp
	EffekseerRendererDX11/EffekseerRenderer/EffekseerRendererDX11.TextureLoader.cpp
	EffekseerRendererDX11/EffekseerRenderer/EffekseerRendererDX11.VertexBuffer.cpp)

set(effekseer_renderer_gl_src
	EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.DeviceObject.cpp
	EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.GLExtension.cpp
	EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.IndexBuffer.cpp
	EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.ModelLoader.cpp
	EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.ModelRenderer.cpp
	EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.Renderer.cpp
	EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.RenderState.cpp
	EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.Shader.cpp
	EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.TextureLoader.cpp
	EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.VertexArray.cpp
	EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.VertexBuffer.cpp)

list(APPEND effekseer_renderer_dx9_src
	${effekseer_renderer_common_src})

list(APPEND effekseer_renderer_dx11_src
	${effekseer_renderer_common_src})

list(APPEND effekseer_renderer_gl_src
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

set(effekseer_sound_al_src
	EffekseerSoundAL/EffekseerSound/EffekseerSoundAL.Sound.cpp
	EffekseerSoundAL/EffekseerSound/EffekseerSoundAL.SoundLoader.cpp
	EffekseerSoundAL/EffekseerSound/EffekseerSoundAL.SoundPlayer.cpp
	EffekseerSoundAL/EffekseerSound/EffekseerSoundAL.SoundVoice.cpp)

set(effekseer_sound_dsound_src
	EffekseerSoundDSound/EffekseerSound/EffekseerSoundDSound.Sound.cpp
	EffekseerSoundDSound/EffekseerSound/EffekseerSoundDSound.SoundLoader.cpp
	EffekseerSoundDSound/EffekseerSound/EffekseerSoundDSound.SoundPlayer.cpp
	EffekseerSoundDSound/EffekseerSound/EffekseerSoundDSound.SoundVoice.cpp)

set(effekseer_sound_xaudio2_src
	EffekseerSoundXAudio2/EffekseerSound/EffekseerSoundXAudio2.Sound.cpp
	EffekseerSoundXAudio2/EffekseerSound/EffekseerSoundXAudio2.SoundLoader.cpp
	EffekseerSoundXAudio2/EffekseerSound/EffekseerSoundXAudio2.SoundPlayer.cpp
	EffekseerSoundXAudio2/EffekseerSound/EffekseerSoundXAudio2.SoundVoice.cpp)

# インクルードの設定
include_directories(
	Effekseer/
	)

# コンパイル先を追加
add_library( effekseer STATIC ${effekseer_src} )

if (WIN32)
add_library( effekseer_renderer_dx9 STATIC ${effekseer_renderer_dx9_src} )
add_library( effekseer_renderer_dx11 STATIC ${effekseer_renderer_dx11_src} )
add_library( effekseer_sound_dsound STATIC ${effekseer_sound_dsound_src} )
if (USE_XAUDIO2)
add_library( effekseer_sound_xaudio2 STATIC ${effekseer_sound_xaudio2_src} )
endif()
endif()

add_library( effekseer_renderer_gl STATIC ${effekseer_renderer_gl_src} )

if (USE_OPENAL)
add_library( effekseer_sound_al STATIC ${effekseer_sound_al_src} )
endif()

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

if (MSVC)
if (MSVC)
set_target_properties(effekseer_sound_dsound PROPERTIES OUTPUT_NAME_DEBUG "EffekseerSoundDSound" )
set_target_properties(effekseer_sound_dsound PROPERTIES OUTPUT_NAME_RELEASE "EffekseerSoundDSound" )
else()
set_target_properties(effekseer_sound_dsound PROPERTIES OUTPUT_NAME "EffekseerSoundDSound" )
endif()
endif()

if (MSVC)
if (MSVC)
set_target_properties(effekseer_sound_dsound PROPERTIES OUTPUT_NAME_DEBUG "EffekseerSoundDSound" )
set_target_properties(effekseer_sound_dsound PROPERTIES OUTPUT_NAME_RELEASE "EffekseerSoundDSound" )
else()
set_target_properties(effekseer_sound_dsound PROPERTIES OUTPUT_NAME "EffekseerSoundDSound" )
endif()
endif()

if (MSVC)
if (MSVC)
set_target_properties(effekseer_sound_xaudio2 PROPERTIES OUTPUT_NAME_DEBUG "EffekseerSoundXAudio2" )
set_target_properties(effekseer_sound_xaudio2 PROPERTIES OUTPUT_NAME_RELEASE "EffekseerSoundXAudio2" )
else()
set_target_properties(effekseer_sound_xaudio2 PROPERTIES OUTPUT_NAME "EffekseerSoundXAudio2" )
endif()
endif()

if (USE_OPENAL)
if (MSVC)
set_target_properties(effekseer_sound_al PROPERTIES OUTPUT_NAME_DEBUG "EffekseerSoundAL" )
set_target_properties(effekseer_sound_al PROPERTIES OUTPUT_NAME_RELEASE "EffekseerSoundAL" )
else()
set_target_properties(effekseer_sound_al PROPERTIES OUTPUT_NAME "EffekseerSoundAL" )
endif()
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

if (MSVC)
install(
	TARGETS effekseer_sound_dsound
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib)
endif()

if (MSVC)
install(
	TARGETS effekseer_sound_xaudio2
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib)
endif()

install(
	TARGETS effekseer_renderer_gl
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib)

if (USE_OPENAL)
install(
	TARGETS effekseer_sound_al
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib)
endif()
