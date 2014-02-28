
#include "../common/UnityPluginInterface.h"

#include <OpenGL/OpenGL.h>
#include <OpenAl/al.h>
#include <OpenAl/alc.h>

#include <Effekseer.h>
#include <EffekseerRendererGL.h>
#include <EffekseerSoundAL.h>

static const int MAX_INSTANCES	= 1600;
static const int MAX_SQUARES	= 3200;

static int						g_DeviceType = -1;

Effekseer::Manager*				g_EffekseerManager = NULL;
EffekseerRenderer::Renderer*	g_EffekseerRenderer = NULL;
EffekseerSound::Sound*			g_EffekseerSound = NULL;
static ALCdevice*				g_alcdev = NULL;
static ALCcontext*				g_alcctx = NULL;


static void InitializeOpenAL();
static void FinalizeOpenAL();
static void InitializeEffekseer();
static void FinalizeEffekseer();

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

static void InitializeEffekseer()
{
	g_EffekseerManager = Effekseer::Manager::Create(MAX_INSTANCES);

	g_EffekseerRenderer = EffekseerRendererGL::Renderer::Create(MAX_SQUARES);
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

static void SetGraphicsDeviceGL(GfxDeviceEventType eventType)
{
	switch (eventType) {
	case kGfxDeviceEventInitialize:
	case kGfxDeviceEventAfterReset:
		InitializeEffekseer();
		break;
	case kGfxDeviceEventBeforeReset:
	case kGfxDeviceEventShutdown:
		FinalizeEffekseer();
		break;
	}
}

extern "C"
{
	void EXPORT_API UnitySetGraphicsDevice(void* device, int deviceType, int eventType)
	{
		g_DeviceType = -1;

		if (deviceType == kGfxRendererOpenGL)
		{
			g_DeviceType = deviceType;
			SetGraphicsDeviceGL((GfxDeviceEventType)eventType);
		}
	}

	void EXPORT_API UnityRenderEvent(int eventID)
	{
		if (g_DeviceType == -1)
			return;
	
		g_EffekseerRenderer->BeginRendering();
		g_EffekseerManager->Draw();
		g_EffekseerRenderer->EndRendering();
	}

	void EXPORT_API EffekseerUpdate(float deltaFrame)
	{
		g_EffekseerManager->Update(deltaFrame);
	}

	void EXPORT_API EffekseerSetProjection(float fov, float aspect, float nearZ, float farZ)
	{
		Effekseer::Matrix44 projectionMatrix;
		projectionMatrix.PerspectiveFovLH_OpenGL(
			fov * 3.1415926f / 180.0f, aspect, nearZ, farZ);
		g_EffekseerRenderer->SetProjectionMatrix(projectionMatrix);
	}

	void EXPORT_API EffekseerSetCamera(float ex, float ey, float ez, 
		float ax, float ay, float az, float ux, float uy, float uz)
	{
		Effekseer::Vector3D eye(ex, ey, ez);
		Effekseer::Vector3D at(ax, ay, az);
		Effekseer::Vector3D up(ux, uy, uz);
	
		Effekseer::Matrix44 cameraMatrix;
		cameraMatrix.LookAtLH(eye, at, up);
		g_EffekseerRenderer->SetCameraMatrix(cameraMatrix);
	}
	
	Effekseer::Effect EXPORT_API *EffekseerLoadEffect(const char* path)
	{
		EFK_CHAR path16[512];
		Effekseer::ConvertUtf8ToUtf16((int16_t*)path16, 512, (int8_t*)path);
		return Effekseer::Effect::Create(g_EffekseerManager, path16);
	}

	void EXPORT_API EffekseerReleaseEffect(Effekseer::Effect* effect)
	{
		if (effect != NULL) {
			effect->Release();
		}
	}
}