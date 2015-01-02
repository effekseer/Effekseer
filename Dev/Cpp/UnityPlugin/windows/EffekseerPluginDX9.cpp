
#include "../common/UnityPluginInterface.h"
#include <windows.h>
#include <shlwapi.h>

#include "Effekseer.h"
#include "EffekseerRendererDX9.h"
#include "EffekseerRendererDX11.h"
#include "EffekseerSoundXAudio2.h"

#pragma comment(lib, "shlwapi.lib")

static const int RENDER_EVENT_ID_GAME	= 0x2040;
static const int RENDER_EVENT_ID_EDITOR	= 0x2041;

static int					g_DeviceType = -1;
static IDirect3DDevice9*	g_D3d9Device = NULL;
static ID3D11Device*		g_D3d11Device = NULL;
static ID3D11DeviceContext*	g_D3d11Context = NULL;

Effekseer::Manager*				g_EffekseerManager = NULL;
EffekseerRenderer::Renderer*	g_EffekseerRenderer = NULL;
EffekseerSound::Sound*			g_EffekseerSound = NULL;
IXAudio2*						g_XAudio2 = NULL;
IXAudio2MasteringVoice*			g_MasteringVoice = NULL;

static Effekseer::Matrix44		g_cameraMatrix[2];
static Effekseer::Matrix44		g_projectionMatrix[2];

static bool InitializeXAudio2();
static void FinalizeXAudio2();
static void InitializeEffekseer(int maxInstances, int maxSquares);
static void FinalizeEffekseer();

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	bool res = true;
	switch (fdwReason) {
	case DLL_PROCESS_ATTACH:
		CoInitializeEx(NULL, COINIT_MULTITHREADED);
		break;
	case DLL_PROCESS_DETACH:
		CoUninitialize();
		break;
	case DLL_THREAD_ATTACH:
		CoInitializeEx(NULL, COINIT_MULTITHREADED);
		break;
	case DLL_THREAD_DETACH:
		CoUninitialize();
		break;
	default:
		break;
	}
	return res;
}

static bool InitializeXAudio2()
{
	HRESULT hr;
	hr = XAudio2Create(&g_XAudio2, 0);
	if (FAILED(hr)) {
		FinalizeXAudio2();
		return false;
	}
	hr = g_XAudio2->CreateMasteringVoice(&g_MasteringVoice, 2, 44100);
	if (FAILED(hr)) {
		FinalizeXAudio2();
		return false;
	}
	return true;
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
}

// Effekseer‰Šú‰»
static void InitializeEffekseer(int maxInstances, int maxSquares)
{
	switch (g_DeviceType) {
	case kGfxRendererD3D9:
		g_EffekseerRenderer = EffekseerRendererDX9::Renderer::Create(g_D3d9Device, maxSquares);
		break;
	case kGfxRendererD3D11:
		g_EffekseerRenderer = EffekseerRendererDX11::Renderer::Create(g_D3d11Device, g_D3d11Context, maxSquares);
		break;
	}

	g_EffekseerManager = Effekseer::Manager::Create(maxInstances);
	g_EffekseerManager->SetSpriteRenderer(g_EffekseerRenderer->CreateSpriteRenderer());
	g_EffekseerManager->SetRibbonRenderer(g_EffekseerRenderer->CreateRibbonRenderer());
	g_EffekseerManager->SetRingRenderer(g_EffekseerRenderer->CreateRingRenderer());
	g_EffekseerManager->SetModelRenderer(g_EffekseerRenderer->CreateModelRenderer());

	g_EffekseerManager->SetTextureLoader(g_EffekseerRenderer->CreateTextureLoader());
	g_EffekseerManager->SetModelLoader(g_EffekseerRenderer->CreateModelLoader());

	if (g_XAudio2 != NULL) {
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
		break;
	case kGfxDeviceEventBeforeReset:
	case kGfxDeviceEventShutdown:
		break;
	}
}

static void SetGraphicsDeviceD3D11(ID3D11Device* device, GfxDeviceEventType eventType)
{
	g_D3d11Device = device;

	switch (eventType) {
	case kGfxDeviceEventInitialize:
	case kGfxDeviceEventAfterReset:
		g_D3d11Device->GetImmediateContext(&g_D3d11Context);
		break;
	case kGfxDeviceEventBeforeReset:
	case kGfxDeviceEventShutdown:
		if (g_D3d11Context != NULL) {
			g_D3d11Context->Release();
			g_D3d11Context = NULL;
		}
		break;
	}
}

inline bool CheckEventId(int eventId)
{
	return eventId >= RENDER_EVENT_ID_GAME && eventId <= RENDER_EVENT_ID_EDITOR;
}

static void Array2Matrix(Effekseer::Matrix44& matrix, float matrixArray[])
{
	matrix.Values[0][0] = matrixArray[ 0];
	matrix.Values[1][0] = matrixArray[ 1];
	matrix.Values[2][0] = matrixArray[ 2];
	matrix.Values[3][0] = matrixArray[ 3];
	matrix.Values[0][1] = matrixArray[ 4];
	matrix.Values[1][1] = matrixArray[ 5];
	matrix.Values[2][1] = matrixArray[ 6];
	matrix.Values[3][1] = matrixArray[ 7];
	matrix.Values[0][2] = matrixArray[ 8];
	matrix.Values[1][2] = matrixArray[ 9];
	matrix.Values[2][2] = matrixArray[10];
	matrix.Values[3][2] = matrixArray[11];
	matrix.Values[0][3] = matrixArray[12];
	matrix.Values[1][3] = matrixArray[13];
	matrix.Values[2][3] = matrixArray[14];
	matrix.Values[3][3] = matrixArray[15];
}

extern "C"
{
	void EXPORT_API UnitySetGraphicsDevice(void* device, int deviceType, int eventType)
	{
		switch (deviceType) {
		case kGfxRendererD3D9:
			g_DeviceType = deviceType;
			SetGraphicsDeviceD3D9((IDirect3DDevice9*)device, (GfxDeviceEventType)eventType);
			break;
		case kGfxRendererD3D11:
			g_DeviceType = deviceType;
			SetGraphicsDeviceD3D11((ID3D11Device*)device, (GfxDeviceEventType)eventType);
			break;
		default:
			break;
		}
	}

	void EXPORT_API UnityRenderEvent(int eventId)
	{
		if (g_DeviceType == -1) return;
		if (!CheckEventId(eventId)) return;
		if (g_EffekseerManager == NULL) return;
		if (g_EffekseerRenderer == NULL) return;
		
		g_EffekseerRenderer->SetProjectionMatrix(g_projectionMatrix[eventId - RENDER_EVENT_ID_GAME]);
		g_EffekseerRenderer->SetCameraMatrix(g_cameraMatrix[eventId - RENDER_EVENT_ID_GAME]);

		g_EffekseerRenderer->BeginRendering();
		g_EffekseerManager->Draw();
		g_EffekseerRenderer->EndRendering();
	}

	void EXPORT_API EffekseerInit(int maxInstances, int maxSquares)
	{
		InitializeXAudio2();
		InitializeEffekseer(maxInstances, maxSquares);
	}

	void EXPORT_API EffekseerTerm()
	{
		FinalizeEffekseer();
		FinalizeXAudio2();
	}
	
	void EXPORT_API EffekseerSetProjectionMatrix(int eventId, float matrixArray[])
	{
		if (!CheckEventId(eventId)) return;
		
		Effekseer::Matrix44& matrix = g_projectionMatrix[eventId - RENDER_EVENT_ID_GAME];
		Array2Matrix(matrix, matrixArray);
		
		// Scale and bias from OpenGL -> D3D depth range
		for (int i = 0; i < 4; i++) {
			matrix.Values[i][2] = matrix.Values[i][2] * 0.5f + matrix.Values[i][3] * 0.5f;
		}
	}

	void EXPORT_API EffekseerSetCameraMatrix(int eventId, float matrixArray[])
	{
		if (!CheckEventId(eventId)) return;

		Effekseer::Matrix44& matrix = g_cameraMatrix[eventId - RENDER_EVENT_ID_GAME];
		Array2Matrix(matrix, matrixArray);
	}
}
