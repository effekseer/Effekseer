#pragma warning (disable : 4005)

#include <windows.h>
#include <shlwapi.h>
#include "Effekseer.h"
#include "EffekseerRendererDX9.h"
#include "EffekseerRendererDX11.h"
#include "../common/EffekseerPluginCommon.h"
#include "../common/IUnityGraphics.h"
#include "../common/IUnityGraphicsD3D9.h"
#include "../common/IUnityGraphicsD3D11.h"

#pragma comment(lib, "shlwapi.lib")

using namespace Effekseer;

IUnityInterfaces*		g_UnityInterfaces = NULL;
IUnityGraphics*			g_Graphics = NULL;
UnityGfxRenderer		g_RendererType = kUnityGfxRendererNull;
IDirect3DDevice9*		g_D3d9Device = NULL;
ID3D11Device*			g_D3d11Device = NULL;
ID3D11DeviceContext*	g_D3d11Context = NULL;

Effekseer::Manager*				g_EffekseerManager = NULL;
EffekseerRenderer::Renderer*	g_EffekseerRenderer = NULL;

static void InitializeEffekseer(int maxInstances, int maxSquares);
static void FinalizeEffekseer();

static void UNITY_INTERFACE_API OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType);
static void OnGraphicsDeviceEventD3D9(UnityGfxDeviceEventType eventType);
static void OnGraphicsDeviceEventD3D11(UnityGfxDeviceEventType eventType);

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

	switch (g_RendererType) {
	case kUnityGfxRendererD3D9:
		OnGraphicsDeviceEventD3D9(eventType);
		break;
	case kUnityGfxRendererD3D11:
		OnGraphicsDeviceEventD3D11(eventType);
		break;
	default:
		break;
	}
}

static void OnGraphicsDeviceEventD3D9(UnityGfxDeviceEventType eventType)
{
	switch (eventType) {
	case kUnityGfxDeviceEventInitialize:
		g_D3d9Device = g_UnityInterfaces->Get<IUnityGraphicsD3D9>()->GetDevice();
		break;
	case kUnityGfxDeviceEventAfterReset:
		break;
	case kUnityGfxDeviceEventBeforeReset:
	case kUnityGfxDeviceEventShutdown:
		break;
	}
}

static void OnGraphicsDeviceEventD3D11(UnityGfxDeviceEventType eventType)
{
	switch (eventType) {
	case kUnityGfxDeviceEventInitialize:
		g_D3d11Device = g_UnityInterfaces->Get<IUnityGraphicsD3D11>()->GetDevice();
		// fall throuth
	case kUnityGfxDeviceEventAfterReset:
		g_D3d11Device->GetImmediateContext(&g_D3d11Context);
		break;
	case kUnityGfxDeviceEventBeforeReset:
	case kUnityGfxDeviceEventShutdown:
		if (g_D3d11Context != NULL) {
			g_D3d11Context->Release();
			g_D3d11Context = NULL;
		}
		break;
	}
}

extern "C"
{
	void UNITY_API EffekseerRender(int renderId)
	{
		if (g_EffekseerManager == NULL) return;
		if (g_EffekseerRenderer == NULL) return;
		
		// 行列をセット
		g_EffekseerRenderer->SetProjectionMatrix(g_projectionMatrix[renderId]);
		g_EffekseerRenderer->SetCameraMatrix(g_cameraMatrix[renderId]);

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
		switch (g_RendererType) {
		case kUnityGfxRendererD3D9:
			g_EffekseerRenderer = EffekseerRendererDX9::Renderer::Create(g_D3d9Device, maxSquares);
			break;
		case kUnityGfxRendererD3D11:
			g_EffekseerRenderer = EffekseerRendererDX11::Renderer::Create(g_D3d11Device, g_D3d11Context, maxSquares);
			break;
		default:
			return;
		}

		g_EffekseerManager = Effekseer::Manager::Create(maxInstances);
		g_EffekseerManager->SetSpriteRenderer(g_EffekseerRenderer->CreateSpriteRenderer());
		g_EffekseerManager->SetRibbonRenderer(g_EffekseerRenderer->CreateRibbonRenderer());
		g_EffekseerManager->SetRingRenderer(g_EffekseerRenderer->CreateRingRenderer());
		g_EffekseerManager->SetModelRenderer(g_EffekseerRenderer->CreateModelRenderer());

		g_EffekseerManager->SetTextureLoader(g_EffekseerRenderer->CreateTextureLoader());
		g_EffekseerManager->SetModelLoader(g_EffekseerRenderer->CreateModelLoader());
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
