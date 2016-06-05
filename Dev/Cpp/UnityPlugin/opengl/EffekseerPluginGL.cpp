

#include "../common/EffekseerPluginCommon.h"
#include "../common/IUnityGraphics.h"

#include "EffekseerPluginGL.h"

#if _WIN32
#pragma comment(lib, "opengl32.lib")
#endif

using namespace Effekseer;
using namespace EffekseerPlugin;

#ifndef _WIN32

namespace EffekseerPlugin
{
	IUnityInterfaces*		g_UnityInterfaces = NULL;
	IUnityGraphics*			g_UnityGraphics = NULL;
	UnityGfxRenderer		g_UnityRendererType = kUnityGfxRendererNull;

	Effekseer::Manager*				g_EffekseerManager = NULL;
	EffekseerRendererGL::Renderer*	g_EffekseerRenderer = NULL;

	void UNITY_API OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType)
	{
		switch (eventType) {
		case kUnityGfxDeviceEventInitialize:
			g_UnityRendererType = g_UnityGraphics->GetRenderer();
			break;
		case kUnityGfxDeviceEventShutdown:
			g_UnityRendererType = kUnityGfxRendererNull;
			break;
		case kUnityGfxDeviceEventBeforeReset:
			break;
		case kUnityGfxDeviceEventAfterReset:
			break;
		}
	}
}

extern "C"
{
	// Unity plugin load event
	DLLEXPORT void UNITY_API UnityPluginLoad(IUnityInterfaces* unityInterfaces)
	{
		g_UnityInterfaces = unityInterfaces;
		g_UnityGraphics = g_UnityInterfaces->Get<IUnityGraphics>();

		g_UnityGraphics->RegisterDeviceEventCallback(OnGraphicsDeviceEvent);

		// Run OnGraphicsDeviceEvent(initialize) manually on plugin load
		// to not miss the event in case the graphics device is already initialized
		OnGraphicsDeviceEvent(kUnityGfxDeviceEventInitialize);
	}

	// Unity plugin unload event
	DLLEXPORT void UNITY_API UnityPluginUnload()
	{
		g_UnityGraphics->UnregisterDeviceEventCallback(OnGraphicsDeviceEvent);
	}

	void UNITY_API EffekseerRender(int renderId)
	{
		if (g_EffekseerManager == NULL) return;
		if (g_EffekseerRenderer == NULL) return;

		const RenderSettings& settings = renderSettings[renderId];

		// 行列をセット
		g_EffekseerRenderer->SetProjectionMatrix(settings.projectionMatrix);
		g_EffekseerRenderer->SetCameraMatrix(settings.cameraMatrix);

		// 描画実行(全体)
		g_EffekseerRenderer->BeginRendering();
		g_EffekseerManager->Draw();
		g_EffekseerRenderer->EndRendering();
	}


	DLLEXPORT UnityRenderingEvent UNITY_API EffekseerGetRenderFunc(int renderId)
	{
		return EffekseerRender;
	}

	DLLEXPORT void UNITY_API EffekseerInit(int maxInstances, int maxSquares)
	{
		g_EffekseerManager = Effekseer::Manager::Create(maxInstances);

		g_EffekseerRenderer = EffekseerRendererGL::Renderer::Create(maxSquares);
		g_EffekseerRenderer->SetDistortingCallback(new DistortingCallbackGL(g_EffekseerRenderer));
		g_EffekseerManager->SetSpriteRenderer(g_EffekseerRenderer->CreateSpriteRenderer());
		g_EffekseerManager->SetRibbonRenderer(g_EffekseerRenderer->CreateRibbonRenderer());
		g_EffekseerManager->SetRingRenderer(g_EffekseerRenderer->CreateRingRenderer());
		g_EffekseerManager->SetModelRenderer(g_EffekseerRenderer->CreateModelRenderer());
	}

	DLLEXPORT void UNITY_API EffekseerTerm()
	{
		if (g_EffekseerManager != NULL) {
			g_EffekseerManager->Destroy();
			g_EffekseerManager = NULL;
		}
		if (g_EffekseerRenderer != NULL) {
			g_EffekseerRenderer->Destory();
			g_EffekseerRenderer = NULL;
		}
	}
}

#endif

namespace EffekseerPlugin
{
	DistortingCallbackGL::DistortingCallbackGL(EffekseerRendererGL::Renderer* renderer)
	{
		this->renderer = renderer;
		glGenTextures( 1, &backGroundTexture );
	}
		
	DistortingCallbackGL::~DistortingCallbackGL()
	{
		ReleaseTexture();
	}
		
	void DistortingCallbackGL::ReleaseTexture()
	{
		glDeleteTextures( 1, &backGroundTexture );
	}
	
	void DistortingCallbackGL::OnDistorting()
	{
		GLint viewport[4];
		glGetIntegerv( GL_VIEWPORT, viewport );
		uint32_t width = viewport[2];
		uint32_t height = viewport[3];
		
		glBindTexture( GL_TEXTURE_2D, backGroundTexture );
		//glCopyTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, 0, 0, width, height );
		glCopyTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, width, height, 0 );
		glBindTexture( GL_TEXTURE_2D, 0 );
		
		renderer->SetBackground(backGroundTexture);
	}
	
}
