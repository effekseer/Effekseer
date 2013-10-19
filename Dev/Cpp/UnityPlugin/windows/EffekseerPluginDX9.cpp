
#include "../common/UnityPluginInterface.h"
#include <windows.h>
#include <shlwapi.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <xaudio2.h>

#include "Effekseer.h"
#include "EffekseerRendererDX9.h"
#include "EffekseerSoundXAudio2.h"

#pragma comment(lib, "shlwapi.lib")

static const int MAX_INSTANCES	= 800;
static const int MAX_SQUARES	= 1200;

static int					g_DeviceType = -1;
static IDirect3DDevice9*	g_D3d9Device = NULL;

Effekseer::Manager*				g_EffekseerManager = NULL;
EffekseerRenderer::Renderer*	g_EffekseerRenderer = NULL;
EffekseerSound::Sound*			g_EffekseerSound = NULL;
IXAudio2*						g_XAudio2 = NULL;
IXAudio2MasteringVoice*			g_MasteringVoice = NULL;


static void InitializeXAudio2();
static void FinalizeXAudio2();
static void InitializeEffekseer();
static void FinalizeEffekseer();

static void InitializeXAudio2()
{
	HRESULT hr;
	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	hr = XAudio2Create(&g_XAudio2, 0);
	if (FAILED(hr)) {
		FinalizeXAudio2();
		return;
	}
	hr = g_XAudio2->CreateMasteringVoice(&g_MasteringVoice, 2, 44100);
	if (FAILED(hr)) {
		FinalizeXAudio2();
		return;
	}
}

static void FinalizeXAudio2()
{
	if (g_MasteringVoice != NULL) {
		g_MasteringVoice->DestroyVoice();
		g_MasteringVoice = NULL;
	}
	if (g_XAudio2 != NULL) {
		g_XAudio2->Release();
		g_XAudio2 = NULL;
	}
	CoUninitialize();
}

// Effekseer‰Šú‰»
static void InitializeEffekseer()
{
	g_EffekseerManager = Effekseer::Manager::Create(MAX_INSTANCES);

	g_EffekseerRenderer = EffekseerRenderer::Renderer::Create(g_D3d9Device, MAX_SQUARES);
	g_EffekseerManager->SetSpriteRenderer(g_EffekseerRenderer->CreateSpriteRenderer());
	g_EffekseerManager->SetRibbonRenderer(g_EffekseerRenderer->CreateRibbonRenderer());
	g_EffekseerManager->SetRingRenderer(g_EffekseerRenderer->CreateRingRenderer());
	g_EffekseerManager->SetModelRenderer(g_EffekseerRenderer->CreateModelRenderer());

	g_EffekseerManager->SetTextureLoader(g_EffekseerRenderer->CreateTextureLoader());
	g_EffekseerManager->SetModelLoader(g_EffekseerRenderer->CreateModelLoader());

	InitializeXAudio2();
	if (g_XAudio2 != NULL)
	{
		g_EffekseerSound = EffekseerSound::Sound::Create(g_XAudio2, 16, 16);
		g_EffekseerManager->SetSoundPlayer(g_EffekseerSound->CreateSoundPlayer());
		g_EffekseerManager->SetSoundLoader(g_EffekseerSound->CreateSoundLoader());
	}
}

// EffekseerI—¹ˆ—
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
	FinalizeXAudio2();
	if (g_EffekseerRenderer != NULL) {
		g_EffekseerRenderer->Destory();
		g_EffekseerRenderer = NULL;
	}
}

static void SetGraphicsDeviceD3D9(IDirect3DDevice9* device, GfxDeviceEventType eventType)
{
	g_D3d9Device = device;

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

		if (deviceType == kGfxRendererD3D9)
		{
			g_DeviceType = deviceType;
			SetGraphicsDeviceD3D9((IDirect3DDevice9*)device, (GfxDeviceEventType)eventType);
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
		projectionMatrix.PerspectiveFovLH(
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
		MultiByteToWideChar(CP_ACP, 0, path, -1, (wchar_t*)path16, MAX_PATH);
		return Effekseer::Effect::Create(g_EffekseerManager, path16);
	}

	void EXPORT_API EffekseerReleaseEffect(Effekseer::Effect* effect)
	{
		if (effect != NULL) {
			effect->Release();
		}
	}
}