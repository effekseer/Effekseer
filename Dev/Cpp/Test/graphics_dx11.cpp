
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "common.h"

#include <windows.h>
#include <d3d11.h>
#include "../EffekseerRendererDX11/EffekseerRendererDX11.h"
#include "graphics.h"
#include "window.h"

#pragma comment(lib, "d3dcompiler.lib")

#if _DEBUG
#pragma comment(lib, "x86/EffekseerRendererDX11.Debug.lib" )
#else
#pragma comment(lib, "x86/EffekseerRendererDX11.Release.lib" )
#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ID3D11Device*			g_device = NULL;
ID3D11DeviceContext*	g_context = NULL;
IDXGIDevice1*			g_dxgiDevice = NULL;
IDXGIAdapter*			g_adapter = NULL;
IDXGIFactory*			g_dxgiFactory = NULL;
IDXGISwapChain*			g_swapChain = NULL;
ID3D11Texture2D*		g_backBuffer = NULL;
ID3D11Texture2D*		g_depthBuffer = NULL;
ID3D11RenderTargetView*	g_renderTargetView = NULL;
ID3D11DepthStencilView*	g_depthStencilView = NULL;
static ::EffekseerRenderer::Renderer*	g_renderer = NULL;

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class DistortingCallbackDX11 : public EffekseerRenderer::DistortingCallback
{
	ID3D11Device*			g_D3d11Device = NULL;
	ID3D11DeviceContext*	g_D3d11Context = NULL;

	::EffekseerRendererDX11::Renderer* renderer = nullptr;
	ID3D11Texture2D* backGroundTexture = nullptr;
	ID3D11ShaderResourceView* backGroundTextureSRV = nullptr;
	D3D11_TEXTURE2D_DESC backGroundTextureDesc;

public:
	DistortingCallbackDX11(::EffekseerRendererDX11::Renderer* renderer)
		: renderer(renderer)
	{
		g_D3d11Device = g_device;
		g_D3d11Context = g_context;
	}

	virtual ~DistortingCallbackDX11()
	{
		ReleaseTexture();
	}

	void ReleaseTexture()
	{
		ES_SAFE_RELEASE(backGroundTextureSRV);
		ES_SAFE_RELEASE(backGroundTexture);
	}

	// コピー先のテクスチャを準備
	void PrepareTexture(uint32_t width, uint32_t height, DXGI_FORMAT format)
	{
		ReleaseTexture();

		ZeroMemory(&backGroundTextureDesc, sizeof(backGroundTextureDesc));
		backGroundTextureDesc.Usage = D3D11_USAGE_DEFAULT;
		backGroundTextureDesc.Format = format;
		backGroundTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		backGroundTextureDesc.Width = width;
		backGroundTextureDesc.Height = height;
		backGroundTextureDesc.CPUAccessFlags = 0;
		backGroundTextureDesc.MipLevels = 1;
		backGroundTextureDesc.ArraySize = 1;
		backGroundTextureDesc.SampleDesc.Count = 1;
		backGroundTextureDesc.SampleDesc.Quality = 0;

		HRESULT hr = S_OK;
		hr = g_D3d11Device->CreateTexture2D(&backGroundTextureDesc, nullptr, &backGroundTexture);
		if (FAILED(hr)){
			return;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(srvDesc));
		switch (format)
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
		hr = g_D3d11Device->CreateShaderResourceView(backGroundTexture, &srvDesc, &backGroundTextureSRV);
		if (FAILED(hr)){
			return;
		}
	}

	virtual bool OnDistorting() override
	{
		HRESULT hr = S_OK;

		ID3D11RenderTargetView* renderTargetView = nullptr;
		ID3D11Texture2D* renderTexture = nullptr;

		g_D3d11Context->OMGetRenderTargets(1, &renderTargetView, nullptr);
		renderTargetView->GetResource(reinterpret_cast<ID3D11Resource**>(&renderTexture));

		D3D11_TEXTURE2D_DESC renderTextureDesc;
		renderTexture->GetDesc(&renderTextureDesc);

		if (backGroundTextureSRV == nullptr ||
			backGroundTextureDesc.Width == renderTextureDesc.Width ||
			backGroundTextureDesc.Height == renderTextureDesc.Height ||
			backGroundTextureDesc.Format == renderTextureDesc.Format)
		{
			PrepareTexture(renderTextureDesc.Width, renderTextureDesc.Height, renderTextureDesc.Format);
		}

		g_D3d11Context->CopyResource(backGroundTexture, renderTexture);
		ES_SAFE_RELEASE(renderTexture);
		ES_SAFE_RELEASE(renderTargetView);

		renderer->SetBackground(backGroundTextureSRV);

		return true;
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void InitGraphics(int width, int height )
{
	InitWindow(width, height);

	UINT debugFlag = 0;
	debugFlag = D3D11_CREATE_DEVICE_DEBUG;

	HRESULT hr = D3D11CreateDevice(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		debugFlag,
		NULL,
		0,
		D3D11_SDK_VERSION,
		&g_device,
		NULL,
		&g_context );

	if FAILED( hr )
	{
		goto End;
	}

	if( FAILED(g_device->QueryInterface(__uuidof(IDXGIDevice1), (void**)&g_dxgiDevice)) )
	{
		goto End;
	}

	if( FAILED(g_dxgiDevice->GetAdapter(&g_adapter)) )
	{
		goto End;
	}

	g_adapter->GetParent( __uuidof(IDXGIFactory), (void**)&g_dxgiFactory );
	if( g_dxgiFactory == NULL )
	{
		goto End;
	}

	DXGI_SWAP_CHAIN_DESC hDXGISwapChainDesc;
	hDXGISwapChainDesc.BufferDesc.Width = width;
	hDXGISwapChainDesc.BufferDesc.Height = height;
	hDXGISwapChainDesc.BufferDesc.RefreshRate.Numerator  = 60;
	hDXGISwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	hDXGISwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	hDXGISwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	hDXGISwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	hDXGISwapChainDesc.SampleDesc.Count = 1;
	hDXGISwapChainDesc.SampleDesc.Quality = 0;
	hDXGISwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	hDXGISwapChainDesc.BufferCount = 1;
	hDXGISwapChainDesc.OutputWindow = (HWND)GetHandle();
	hDXGISwapChainDesc.Windowed = TRUE;
	hDXGISwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	hDXGISwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	if( FAILED(g_dxgiFactory->CreateSwapChain(g_device, &hDXGISwapChainDesc, &g_swapChain)) )
	{
		goto End;
	}

	if( FAILED( g_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&g_backBuffer)) )
	{
		goto End;
	}

	if( FAILED(g_device->CreateRenderTargetView(g_backBuffer, NULL, &g_renderTargetView)) )
	{
		goto End;
	}

	D3D11_TEXTURE2D_DESC hTexture2dDesc;
	hTexture2dDesc.Width			= hDXGISwapChainDesc.BufferDesc.Width;
	hTexture2dDesc.Height			= hDXGISwapChainDesc.BufferDesc.Height;
	hTexture2dDesc.MipLevels		= 1;
	hTexture2dDesc.ArraySize		= 1;
	hTexture2dDesc.Format			= DXGI_FORMAT_D24_UNORM_S8_UINT;
	hTexture2dDesc.SampleDesc		= hDXGISwapChainDesc.SampleDesc;
	hTexture2dDesc.Usage			= D3D11_USAGE_DEFAULT;
	hTexture2dDesc.BindFlags		= D3D11_BIND_DEPTH_STENCIL;
    hTexture2dDesc.CPUAccessFlags	= 0;
    hTexture2dDesc.MiscFlags		= 0;
	if(FAILED(g_device->CreateTexture2D(&hTexture2dDesc, NULL, &g_depthBuffer)))
	{
		goto End;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC hDepthStencilViewDesc;
	hDepthStencilViewDesc.Format		= hTexture2dDesc.Format;
	hDepthStencilViewDesc.ViewDimension	= D3D11_DSV_DIMENSION_TEXTURE2DMS;
	hDepthStencilViewDesc.Flags			= 0;
	if(FAILED(g_device->CreateDepthStencilView(g_depthBuffer, &hDepthStencilViewDesc, &g_depthStencilView)))
	{
		goto End;
	}

	g_context->OMSetRenderTargets(1, &g_renderTargetView, g_depthStencilView);

	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = (float)width;
	vp.Height = (float)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	g_context->RSSetViewports(1, &vp);
	
	g_renderer = ::EffekseerRendererDX11::Renderer::Create( g_device, g_context, 16384 );
	g_renderer->SetDistortingCallback(new DistortingCallbackDX11((::EffekseerRendererDX11::Renderer*)g_renderer));

	g_renderer->SetProjectionMatrix( ::Effekseer::Matrix44().PerspectiveFovRH( 90.0f / 180.0f * 3.14f, (float)width / (float)height, 1.0f, 50.0f ) );
	
	g_manager->SetSpriteRenderer( g_renderer->CreateSpriteRenderer() );
	g_manager->SetRibbonRenderer( g_renderer->CreateRibbonRenderer() );
	g_manager->SetRingRenderer( g_renderer->CreateRingRenderer() );
	g_manager->SetModelRenderer( g_renderer->CreateModelRenderer() );
	g_manager->SetTrackRenderer( g_renderer->CreateTrackRenderer() );

	g_manager->SetCoordinateSystem( ::Effekseer::CoordinateSystem::RH );

	g_manager->SetTextureLoader( g_renderer->CreateTextureLoader() );
	g_manager->SetModelLoader( g_renderer->CreateModelLoader() );

	return;
End:
	ES_SAFE_RELEASE( g_renderTargetView );
	ES_SAFE_RELEASE( g_backBuffer );
	ES_SAFE_RELEASE( g_depthStencilView );
	ES_SAFE_RELEASE( g_depthBuffer );
	ES_SAFE_RELEASE( g_swapChain );
	ES_SAFE_RELEASE( g_dxgiFactory );
	ES_SAFE_RELEASE( g_adapter );
	ES_SAFE_RELEASE( g_dxgiDevice );
	ES_SAFE_RELEASE( g_context );
	ES_SAFE_RELEASE( g_device );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void TermGraphics()
{
	g_renderer->Destroy();
	ES_SAFE_RELEASE( g_renderTargetView );
	ES_SAFE_RELEASE( g_backBuffer );
	ES_SAFE_RELEASE( g_depthStencilView );
	ES_SAFE_RELEASE( g_depthBuffer );
	ES_SAFE_RELEASE( g_swapChain );
	ES_SAFE_RELEASE( g_dxgiFactory );
	ES_SAFE_RELEASE( g_adapter );
	ES_SAFE_RELEASE( g_dxgiDevice );
	ES_SAFE_RELEASE( g_context );
	ES_SAFE_RELEASE( g_device );

	ExitWindow();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Rendering()
{
	float ClearColor[] = {0.0f, 0.0f, 0.0f, 1.0f};
	g_context->ClearRenderTargetView( g_renderTargetView, ClearColor);
	g_context->ClearDepthStencilView( g_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	
	g_renderer->SetLightDirection(::Effekseer::Vector3D(1.0f, 1.0f, 1.0f));
	g_renderer->SetLightAmbientColor(::Effekseer::Color(40, 40, 40, 255));

	g_renderer->BeginRendering();

	auto cameraproj = g_renderer->GetCameraProjectionMatrix();
	g_manager->CalcCulling(cameraproj, false);

	g_manager->Draw();
	
	g_renderer->EndRendering();

	g_swapChain->Present(1, 0);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool DoEvent()
{
	return DoWindowEvent();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SetCameraMatrix( const ::Effekseer::Matrix44& matrix )
{
	g_renderer->SetCameraMatrix( matrix );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------