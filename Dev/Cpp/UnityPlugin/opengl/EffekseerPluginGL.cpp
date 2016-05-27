

#include "../common/EffekseerPluginCommon.h"
#include "../common/IUnityGraphics.h"

#include "EffekseerPluginGL.h"

#if _WIN32
#pragma comment(lib, "opengl32.lib")
#endif


using namespace Effekseer;
using namespace EffekseerPlugin;



#pragma region FuncEx
static bool g_isInitialized = false;

#if _WIN32
#define GET_PROC(name)	g_##name = (FP_##name)wglGetProcAddress( #name ); if(g_##name==NULL) return false;
#elif defined(__EFFEKSEER_RENDERER_GLES2__) || defined(__EFFEKSEER_RENDERER_GLES3__)
#define GET_PROC(name)	g_##name = (FP_##name)eglGetProcAddress( #name ); if(g_##name==NULL) return false;
#endif

typedef void (EFK_STDCALL * FP_glGetRenderbufferParameteriv) (GLenum target, GLenum pname, GLint * params);
static FP_glGetRenderbufferParameteriv g_glGetRenderbufferParameteriv = nullptr;

static bool Initialize()
{
	if (g_isInitialized) return true;

#if _WIN32
	GET_PROC(glGetRenderbufferParameteriv);
#endif
}

void glGetRenderbufferParameteriv(GLenum target, GLenum pname, GLint * params)
{
#if _WIN32
	return g_glGetRenderbufferParameteriv(target, pname, params);
#else
	return ::glGetRenderbufferParameteriv(target, pname, params);
#endif
}

#pragma endregion



#ifdef _WIN32

#define GL_RGBA16F                        0x881A
#define GL_RENDERBUFFER                   0x8D41
#define GL_RENDERBUFFER_WIDTH             0x8D42
#define GL_RENDERBUFFER_HEIGHT            0x8D43
#define GL_RENDERBUFFER_INTERNAL_FORMAT   0x8D44

#else
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
		Initialize();

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
		
	// コピー先のテクスチャを準備
	void DistortingCallbackGL::PrepareTexture(uint32_t width, uint32_t height, GLint internalFormat)
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
		
	void DistortingCallbackGL::OnDistorting()
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
		
		renderer->SetBackground(backGroundTexture);
	}
	
}
