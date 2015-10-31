
#include <OpenGL/OpenGL.h>
#include <OpenAl/al.h>
#include <OpenAl/alc.h>

#include <Effekseer.h>
#include <EffekseerRendererGL.h>
#include <EffekseerSoundAL.h>

#include "../common/EffekseerPluginCommon.h"
#include "../common/IUnityGraphics.h"

static IUnityInterfaces*	g_UnityInterfaces = NULL;
static IUnityGraphics*		g_Graphics = NULL;
static UnityGfxRenderer		g_RendererType = kUnityGfxRendererNull;

Effekseer::Manager*				g_EffekseerManager = NULL;
EffekseerRenderer::Renderer*	g_EffekseerRenderer = NULL;
EffekseerSound::Sound*			g_EffekseerSound = NULL;
static ALCdevice*				g_alcdev = NULL;
static ALCcontext*				g_alcctx = NULL;

static void InitializeOpenAL();
static void FinalizeOpenAL();
static void InitializeEffekseer();
static void FinalizeEffekseer();

static void UNITY_API OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType);

static void InitializeOpenAL()
{
	g_alcdev = alcOpenDevice(NULL);
	g_alcctx = alcCreateContext(g_alcdev, NULL);
	alcMakeContextCurrent(g_alcctx);
}

static void FinalizeOpenAL()
{
	if (g_alcctx) {
		alcDestroyContext(g_alcctx);
		g_alcctx = NULL;
	}
	if (g_alcdev) {
		alcCloseDevice(g_alcdev);
		g_alcdev = NULL;
	}
}

static void InitializeEffekseer(int maxInstances, int maxSquares)
{
	g_EffekseerManager = Effekseer::Manager::Create(maxInstances);

	g_EffekseerRenderer = EffekseerRendererGL::Renderer::Create(maxSquares);
	g_EffekseerManager->SetSpriteRenderer(g_EffekseerRenderer->CreateSpriteRenderer());
	g_EffekseerManager->SetRibbonRenderer(g_EffekseerRenderer->CreateRibbonRenderer());
	g_EffekseerManager->SetRingRenderer(g_EffekseerRenderer->CreateRingRenderer());
	g_EffekseerManager->SetModelRenderer(g_EffekseerRenderer->CreateModelRenderer());

	g_EffekseerManager->SetTextureLoader(g_EffekseerRenderer->CreateTextureLoader());
	g_EffekseerManager->SetModelLoader(g_EffekseerRenderer->CreateModelLoader());

	InitializeOpenAL();
	
	g_EffekseerSound = EffekseerSound::Sound::Create(32);
	g_EffekseerManager->SetSoundPlayer(g_EffekseerSound->CreateSoundPlayer());
	g_EffekseerManager->SetSoundLoader(g_EffekseerSound->CreateSoundLoader());
}

static void FinalizeEffekseer()
{
	if (g_EffekseerManager != NULL) {
		g_EffekseerManager->Destroy();
		g_EffekseerManager = NULL;
	}
	if (g_EffekseerSound != NULL) {
		g_EffekseerSound->Destory();
		g_EffekseerSound = NULL;
	}
	FinalizeOpenAL();
	
	if (g_EffekseerRenderer != NULL) {
		g_EffekseerRenderer->Destory();
		g_EffekseerRenderer = NULL;
	}
}

// Unity plugin load event
extern "C" DLLEXPORT void UNITY_API UnityPluginLoad(IUnityInterfaces* unityInterfaces)
{
	g_UnityInterfaces = unityInterfaces;
	g_Graphics = unityInterfaces->Get<IUnityGraphics>();
	
	g_Graphics->RegisterDeviceEventCallback(OnGraphicsDeviceEvent);

	// Run OnGraphicsDeviceEvent(initialize) manually on plugin load
	// to not miss the event in case the graphics device is already initialized
	OnGraphicsDeviceEvent(kUnityGfxDeviceEventInitialize);
}

// Unity plugin unload event
extern "C" DLLEXPORT void UNITY_API UnityPluginUnload()
{
	g_Graphics->UnregisterDeviceEventCallback(OnGraphicsDeviceEvent);
}

static void UNITY_API OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType)
{
	switch (eventType) {
	case kUnityGfxDeviceEventInitialize:
		g_RendererType = g_Graphics->GetRenderer();
		break;
	case kUnityGfxDeviceEventShutdown:
		g_RendererType = kUnityGfxRendererNull;
		break;
	case kUnityGfxDeviceEventBeforeReset:
		break;
	case kUnityGfxDeviceEventAfterReset:
		break;
	}
}

extern "C"
{
	void UNITY_API EffekseerRender(int renderId)
	{
		if (g_EffekseerManager == NULL) return;
		if (g_EffekseerRenderer == NULL) return;
		
		g_EffekseerRenderer->SetProjectionMatrix(g_projectionMatrix[renderId]);
		g_EffekseerRenderer->SetCameraMatrix(g_cameraMatrix[renderId]);

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
		InitializeEffekseer(maxInstances, maxSquares);
	}

	DLLEXPORT void UNITY_API EffekseerTerm()
	{
		FinalizeEffekseer();
	}
}