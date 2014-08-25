
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

static const int RENDER_EVENT_ID	= 0x2040;

static int					g_DeviceType = -1;
static IDirect3DDevice9*	g_D3d9Device = NULL;

Effekseer::Manager*				g_EffekseerManager = NULL;
EffekseerRenderer::Renderer*	g_EffekseerRenderer = NULL;
EffekseerSound::Sound*			g_EffekseerSound = NULL;
IXAudio2*						g_XAudio2 = NULL;
IXAudio2MasteringVoice*			g_MasteringVoice = NULL;


static void InitializeXAudio2();
static void FinalizeXAudio2();
static void InitializeEffekseer(int maxInstances, int maxSquares);
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
static void InitializeEffekseer(int maxInstances, int maxSquares)
{
	g_EffekseerManager = Effekseer::Manager::Create(maxInstances);

	g_EffekseerRenderer = EffekseerRendererDX9::Renderer::Create(g_D3d9Device, maxSquares);
	g_EffekseerManager->SetSpriteRenderer(g_EffekseerRenderer->CreateSpriteRenderer());
	g_EffekseerManager->SetRibbonRenderer(g_EffekseerRenderer->CreateRibbonRenderer());
	g_EffekseerManager->SetRingRenderer(g_EffekseerRenderer->CreateRingRenderer());
	g_EffekseerManager->SetModelRenderer(g_EffekseerRenderer->CreateModelRenderer());

	g_EffekseerManager->SetTextureLoader(g_EffekseerRenderer->CreateTextureLoader());
	g_EffekseerManager->SetModelLoader(g_EffekseerRenderer->CreateModelLoader());

	InitializeXAudio2();
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
		break;
	case kGfxDeviceEventBeforeReset:
	case kGfxDeviceEventShutdown:
		break;
	}
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
		g_DeviceType = -1;
		
		if (deviceType == kGfxRendererD3D9) {
			g_DeviceType = deviceType;
			SetGraphicsDeviceD3D9((IDirect3DDevice9*)device, (GfxDeviceEventType)eventType);
		}
	}

	void EXPORT_API UnityRenderEvent(int eventID)
	{
		if (g_DeviceType == -1) {
			return;
		}
		if (g_EffekseerManager == NULL || g_EffekseerRenderer == NULL) {
			return;
		}
		if (eventID != RENDER_EVENT_ID) {
			return;
		}
		
		g_EffekseerRenderer->BeginRendering();
		g_EffekseerManager->Draw();
		g_EffekseerRenderer->EndRendering();
	}

	void EXPORT_API EffekseerInit(int maxInstances, int maxSquares)
	{
		InitializeEffekseer(maxInstances, maxSquares);
	}

	void EXPORT_API EffekseerTerm()
	{
		FinalizeEffekseer();
	}
	
	void EXPORT_API EffekseerSetProjectionMatrix(float matrixArray[])
	{
		if (g_EffekseerRenderer == NULL) {
			return;
		}

		Effekseer::Matrix44 matrix;
		Array2Matrix(matrix, matrixArray);
		
		// Scale and bias from OpenGL -> D3D depth range
		for (int i = 0; i < 4; i++) {
			matrix.Values[i][2] = matrix.Values[i][2] * 0.5f + matrix.Values[i][3] * 0.5f;
		}

		g_EffekseerRenderer->SetProjectionMatrix(matrix);
	}

	void EXPORT_API EffekseerSetCameraMatrix(float matrixArray[])
	{
		if (g_EffekseerRenderer == NULL) {
			return;
		}

		Effekseer::Matrix44 matrix;
		Array2Matrix(matrix, matrixArray);
		g_EffekseerRenderer->SetCameraMatrix(matrix);
	}
}