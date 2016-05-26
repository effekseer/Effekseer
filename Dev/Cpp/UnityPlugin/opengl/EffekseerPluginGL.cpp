
#include <Effekseer.h>
#include <EffekseerRendererGL.h>
#include "../common/EffekseerPluginCommon.h"
#include "../common/IUnityGraphics.h"

using namespace Effekseer;
using namespace EffekseerPlugin;

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
	
	class DistortingCallback : public EffekseerRenderer::DistortingCallback
	{
		GLuint backGroundTexture = 0;
		uint32_t backGroundTextureWidth = 0;
		uint32_t backGroundTextureHeight = 0;
		GLuint backGroundTextureInternalFormat = 0;
		
	public:
		DistortingCallback() {
			glGenTextures( 1, &backGroundTexture );
		}
		
		virtual ~DistortingCallback()
		{
			ReleaseTexture();
		}
		
		void ReleaseTexture()
		{
			glDeleteTextures( 1, &backGroundTexture );
		}
		
		// コピー先のテクスチャを準備
		void PrepareTexture( uint32_t width, uint32_t height, GLint internalFormat )
		{
			ReleaseTexture();
			
			backGroundTextureWidth = width;
			backGroundTextureHeight = height;
			backGroundTextureInternalFormat = internalFormat;
			
			GLenum format, type;
			switch (internalFormat) {
				case GL_RGBA8:
					format = GL_RGBA;
					type = GL_UNSIGNED_BYTE;
					break;
				case GL_RGBA16F:
					format = GL_RGBA;
					type = GL_UNSIGNED_SHORT;
					break;
				default:
					return;
			}
			glBindTexture( GL_TEXTURE_2D, backGroundTexture );
			glTexImage2D( GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, 0 );
		}
		
		virtual void OnDistorting()
		{
			uint32_t width;
			uint32_t height;
			GLint internalFormat;
			
			glGetRenderbufferParameteriv( GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, (GLint*)&width );
			glGetRenderbufferParameteriv( GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, (GLint*)&height );
			glGetRenderbufferParameteriv( GL_RENDERBUFFER, GL_RENDERBUFFER_INTERNAL_FORMAT, &internalFormat);
			
			if( backGroundTexture == 0 ||
			   backGroundTextureWidth != width ||
			   backGroundTextureHeight != height ||
			   backGroundTextureInternalFormat != internalFormat )
			{
				PrepareTexture( width, height, internalFormat );
			}
			
			glBindTexture( GL_TEXTURE_2D, backGroundTexture );
			glCopyTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, 0, 0, width, height );
			glBindTexture( GL_TEXTURE_2D, 0 );
			
			g_EffekseerRenderer->SetBackground( backGroundTexture );
		}
	};
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
		g_EffekseerRenderer->SetDistortingCallback(new DistortingCallback());
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