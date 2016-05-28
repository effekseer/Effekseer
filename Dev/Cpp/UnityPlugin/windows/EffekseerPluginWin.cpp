#pragma warning (disable : 4005)

#include <assert.h>
#include <windows.h>
#include <shlwapi.h>

#include "Effekseer.h"

#include "EffekseerRendererGL.h"
#include "EffekseerRendererDX9.h"
#include "EffekseerRendererDX11.h"

#include "../common/EffekseerPluginCommon.h"
#include "../common/IUnityGraphics.h"
#include "../common/IUnityGraphicsD3D9.h"
#include "../common/IUnityGraphicsD3D11.h"

#include "../opengl/EffekseerPluginGL.h"

#pragma comment(lib, "shlwapi.lib")

using namespace Effekseer;
using namespace EffekseerPlugin;

namespace EffekseerPlugin
{
	IUnityInterfaces*		g_UnityInterfaces = NULL;
	IUnityGraphics*			g_UnityGraphics = NULL;
	UnityGfxRenderer		g_UnityRendererType = kUnityGfxRendererNull;
	IDirect3DDevice9*		g_D3d9Device = NULL;
	ID3D11Device*			g_D3d11Device = NULL;
	ID3D11DeviceContext*	g_D3d11Context = NULL;

	Effekseer::Manager*				g_EffekseerManager = NULL;
	EffekseerRenderer::Renderer*	g_EffekseerRenderer = NULL;

	void OnGraphicsDeviceEventD3D9(UnityGfxDeviceEventType eventType)
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

	void OnGraphicsDeviceEventD3D11(UnityGfxDeviceEventType eventType)
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

	void OnGraphicsDeviceEventOpenGL(UnityGfxDeviceEventType eventType)
	{
		switch (eventType) {
		case kUnityGfxDeviceEventInitialize:
			g_UnityRendererType = g_UnityGraphics->GetRenderer();
			break;
		case kUnityGfxDeviceEventShutdown:
			g_UnityRendererType = kUnityGfxRendererNull;
			break;
		}
	}

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

		switch (g_UnityRendererType) {
		case kUnityGfxRendererD3D9:
			OnGraphicsDeviceEventD3D9(eventType);
			break;
		case kUnityGfxRendererD3D11:
			OnGraphicsDeviceEventD3D11(eventType);
			break;
		case kUnityGfxRendererOpenGLCore:
			OnGraphicsDeviceEventOpenGL(eventType);
			break;
		default:
			break;
		}
	}
	
	// DirectX9のEffekseerレンダラを作成
	EffekseerRenderer::Renderer* CreateRendererOpenGL(int squareMaxCount)
	{
		auto renderer = EffekseerRendererGL::Renderer::Create(squareMaxCount);
		renderer->SetDistortingCallback(new DistortingCallbackGL(renderer));
		return renderer;
	}


	// DirectX9のEffekseerレンダラを作成
	EffekseerRenderer::Renderer* CreateRendererDX9(int squareMaxCount)
	{
		class DistortingCallback : public EffekseerRenderer::DistortingCallback
		{
			::EffekseerRendererDX9::Renderer* renderer = nullptr;
			IDirect3DTexture9* backGroundTexture = nullptr;
			uint32_t backGroundTextureWidth = 0, backGroundTextureHeight = 0;
			D3DFORMAT backGroundTextureFormat;

		public:
			DistortingCallback( ::EffekseerRendererDX9::Renderer* renderer )
				: renderer( renderer )
			{
			}

			virtual ~DistortingCallback()
			{
				ReleaseTexture();
			}

			void ReleaseTexture()
			{
				ES_SAFE_RELEASE( backGroundTexture );
			}

			// コピー先のテクスチャを準備
			void PrepareTexture( uint32_t width, uint32_t height, D3DFORMAT format )
			{
				ReleaseTexture();

				backGroundTextureWidth = width;
				backGroundTextureHeight = height;
				backGroundTextureFormat = format;
				HRESULT hr = S_OK;
				hr = g_D3d9Device->CreateTexture( width, height, 0, D3DUSAGE_RENDERTARGET, format, D3DPOOL_DEFAULT, &backGroundTexture, nullptr );
				if( FAILED( hr ) ){
					return;
				}
			}

			virtual void OnDistorting()
			{
				IDirect3DSurface9* targetSurface = nullptr;
				IDirect3DSurface9* texSurface = nullptr;
				HRESULT hr = S_OK;

				hr = g_D3d9Device->GetRenderTarget( 0, &targetSurface );
				if( FAILED( hr ) ){
					return;
				}
				
				D3DSURFACE_DESC targetSurfaceDesc;
				targetSurface->GetDesc( &targetSurfaceDesc );
				
				if( backGroundTexture == nullptr || 
					backGroundTextureWidth == targetSurfaceDesc.Width || 
					backGroundTextureHeight == targetSurfaceDesc.Height || 
					backGroundTextureFormat == targetSurfaceDesc.Format )
				{
					PrepareTexture( targetSurfaceDesc.Width, targetSurfaceDesc.Height, targetSurfaceDesc.Format );
				}
				
				hr = backGroundTexture->GetSurfaceLevel( 0, &texSurface );
				if( FAILED( hr ) ){
					return;
				}
				
				hr = g_D3d9Device->StretchRect( targetSurface, NULL, texSurface, NULL, D3DTEXF_NONE);
				if( FAILED( hr ) ){
					return;
				}
				
				ES_SAFE_RELEASE( texSurface );
				ES_SAFE_RELEASE( targetSurface );

				renderer->SetBackground( backGroundTexture );
			}
		};
		
		auto renderer = EffekseerRendererDX9::Renderer::Create(g_D3d9Device, squareMaxCount);
		renderer->SetDistortingCallback( new DistortingCallback( renderer ) );
		return renderer;
	}

	// DirectX11のEffekseerレンダラを作成
	EffekseerRenderer::Renderer* CreateRendererDX11(int squareMaxCount)
	{
		class DistortingCallback : public EffekseerRenderer::DistortingCallback
		{
			::EffekseerRendererDX11::Renderer* renderer = nullptr;
			ID3D11Texture2D* backGroundTexture = nullptr;
			ID3D11ShaderResourceView* backGroundTextureSRV = nullptr;
			D3D11_TEXTURE2D_DESC backGroundTextureDesc;

		public:
			DistortingCallback( ::EffekseerRendererDX11::Renderer* renderer )
				: renderer( renderer )
			{
			}

			virtual ~DistortingCallback()
			{
				ReleaseTexture();
			}

			void ReleaseTexture()
			{
				ES_SAFE_RELEASE( backGroundTextureSRV );
				ES_SAFE_RELEASE( backGroundTexture );
			}

			// コピー先のテクスチャを準備
			void PrepareTexture( uint32_t width, uint32_t height, DXGI_FORMAT format )
			{
				ReleaseTexture();

				ZeroMemory( &backGroundTextureDesc, sizeof(backGroundTextureDesc) );
				backGroundTextureDesc.Usage              = D3D11_USAGE_DEFAULT;
				backGroundTextureDesc.Format             = format;
				backGroundTextureDesc.BindFlags          = D3D11_BIND_SHADER_RESOURCE;
				backGroundTextureDesc.Width              = width;
				backGroundTextureDesc.Height             = height;
				backGroundTextureDesc.CPUAccessFlags     = 0;
				backGroundTextureDesc.MipLevels          = 1;
				backGroundTextureDesc.ArraySize          = 1;
				backGroundTextureDesc.SampleDesc.Count   = 1;
				backGroundTextureDesc.SampleDesc.Quality = 0;
				
				HRESULT hr = S_OK;
				hr = g_D3d11Device->CreateTexture2D( &backGroundTextureDesc, nullptr, &backGroundTexture );
				if( FAILED( hr ) ){
					return;
				}

				D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
				ZeroMemory(&srvDesc, sizeof(srvDesc));
				switch( format )
				{
					case DXGI_FORMAT_R8G8B8A8_TYPELESS:
						srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
						break;
					case DXGI_FORMAT_R16G16B16A16_TYPELESS:
						srvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
						break;
					default:
						srvDesc.Format = format;
						break;
				}
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				srvDesc.Texture2D.MipLevels = 1;
				hr = g_D3d11Device->CreateShaderResourceView( backGroundTexture, &srvDesc, &backGroundTextureSRV );
				if( FAILED( hr ) ){
					return;
				}
			}

			virtual void OnDistorting()
			{
				HRESULT hr = S_OK;

				ID3D11RenderTargetView* renderTargetView = nullptr;
				ID3D11Texture2D* renderTexture = nullptr;
				
				g_D3d11Context->OMGetRenderTargets( 1, &renderTargetView, nullptr );
				renderTargetView->GetResource( reinterpret_cast<ID3D11Resource**>( &renderTexture ) );

				D3D11_TEXTURE2D_DESC renderTextureDesc;
				renderTexture->GetDesc( &renderTextureDesc );
				
				if( backGroundTextureSRV == nullptr || 
					backGroundTextureDesc.Width == renderTextureDesc.Width || 
					backGroundTextureDesc.Height == renderTextureDesc.Height || 
					backGroundTextureDesc.Format == renderTextureDesc.Format )
				{
					PrepareTexture( renderTextureDesc.Width, renderTextureDesc.Height, renderTextureDesc.Format );
				}

				g_D3d11Context->CopyResource( backGroundTexture, renderTexture );
				Sleep(4);
				ES_SAFE_RELEASE( renderTexture );
				ES_SAFE_RELEASE( renderTargetView );
				
				renderer->SetBackground( backGroundTextureSRV );
			}
		};
		
		auto renderer = EffekseerRendererDX11::Renderer::Create( g_D3d11Device, g_D3d11Context, squareMaxCount );
		renderer->SetDistortingCallback( new DistortingCallback( renderer ) );
		return renderer;
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
		Effekseer::Matrix44 projectionMatrix = settings.projectionMatrix;
		if (settings.renderIntoTexture) {
			// テクスチャに対してレンダリングするときは上下反転させる
			projectionMatrix.Values[1][1] = -projectionMatrix.Values[1][1];
		}

		// 行列をセット
		g_EffekseerRenderer->SetProjectionMatrix(projectionMatrix);
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
		switch (g_UnityRendererType) {
		case kUnityGfxRendererD3D9:
			g_EffekseerRenderer = EffekseerPlugin::CreateRendererDX9( maxSquares );
			break;
		case kUnityGfxRendererD3D11:
			g_EffekseerRenderer = EffekseerPlugin::CreateRendererDX11( maxSquares );
			break;
		case kUnityGfxRendererOpenGLCore:
			g_EffekseerRenderer = EffekseerPlugin::CreateRendererOpenGL(maxSquares);
			break;
		default:
			return;
		}

		g_EffekseerManager = Effekseer::Manager::Create(maxInstances);
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