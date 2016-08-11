

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
		g_EffekseerManager->SetTrackRenderer(g_EffekseerRenderer->CreateTrackRenderer());
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
#ifndef _WIN32
		glGenFramebuffers( 1, &framebufferForCopy );
#endif
	}
		
	DistortingCallbackGL::~DistortingCallbackGL()
	{
		ReleaseTexture();
	}
		
	void DistortingCallbackGL::ReleaseTexture()
	{
#ifndef _WIN32
		glDeleteFramebuffers( 1, &framebufferForCopy );
#endif
		glDeleteTextures( 1, &backGroundTexture );
	}
	
	void DistortingCallbackGL::PrepareTexture( uint32_t width, uint32_t height, GLint internalFormat )
	{
		glBindTexture( GL_TEXTURE_2D, backGroundTexture );
		glTexImage2D( GL_TEXTURE_2D, 0, internalFormat, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0 );
		
		backGroundTextureWidth = width;
		backGroundTextureHeight = height;
		backGroundTextureInternalFormat = internalFormat;
	}
	
	void DistortingCallbackGL::OnDistorting()
	{
		GLint viewport[4];
		glGetIntegerv( GL_VIEWPORT, viewport );
		uint32_t width = viewport[2];
		uint32_t height = viewport[3];
		
		if( backGroundTextureWidth != width ||
			backGroundTextureHeight != height )
		{
			PrepareTexture( width, height, GL_RGBA );
		}
		
#ifndef _WIN32
		GLint backupFramebuffer;
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &backupFramebuffer);
		
		GLint rbtype;
		glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &rbtype);
		
		if( rbtype == GL_RENDERBUFFER ){
			GLint renderbuffer;
			glGetFramebufferAttachmentParameteriv( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &renderbuffer );
			
			glBindFramebuffer( GL_FRAMEBUFFER, framebufferForCopy );
			glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderbuffer );
		}else if( rbtype == GL_TEXTURE_2D ){
			GLint renderTexture;
			glGetFramebufferAttachmentParameteriv( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &renderTexture );
			
			glBindFramebuffer( GL_FRAMEBUFFER, framebufferForCopy );
			glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTexture, 0 );
		}
#endif
		
		glBindTexture( GL_TEXTURE_2D, backGroundTexture );
		//glCopyTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, 0, 0, width, height );
		glCopyTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, viewport[0], viewport[1], width, height );
		glBindTexture( GL_TEXTURE_2D, 0 );
		
#ifndef _WIN32
		glBindFramebuffer( GL_FRAMEBUFFER, backupFramebuffer );
#endif

		renderer->SetBackground(backGroundTexture);
	}
	
}
