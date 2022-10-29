#include "RenderingEnvironmentDX11.h"
#include <Effekseer.h>
#include <EffekseerRendererDX11.h>

#include "../../EffekseerRendererDX11/EffekseerRenderer/GraphicsDevice.h"

RenderingEnvironmentDX11::RenderingEnvironmentDX11(std::array<int32_t, 2> windowSize, const char* title)
	: RenderingEnvironment(false, windowSize, title)
{
	UINT debugFlag = 0;
#if _DEBUG
	debugFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, debugFlag, NULL, 0, D3D11_SDK_VERSION, &device_, NULL, &context_);

	if (FAILED(hr))
	{
		throw "Failed : D3D11CreateDevice";
	}

	if (FAILED(device_->QueryInterface(__uuidof(IDXGIDevice1), (void**)&dxgiDevice_)))
	{
		throw "Failed : QueryInterface";
	}

	if (FAILED(dxgiDevice_->GetAdapter(&adapter_)))
	{
		throw "Failed : GetAdapter";
	}

	adapter_->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory_);
	if (dxgiFactory_ == NULL)
	{
		throw "Failed : GetParent";
	}

	DXGI_SWAP_CHAIN_DESC hDXGISwapChainDesc;
	hDXGISwapChainDesc.BufferDesc.Width = windowSize[0];
	hDXGISwapChainDesc.BufferDesc.Height = windowSize[1];
	hDXGISwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	hDXGISwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	hDXGISwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	hDXGISwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	hDXGISwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	hDXGISwapChainDesc.SampleDesc.Count = 1;
	hDXGISwapChainDesc.SampleDesc.Quality = 0;
	hDXGISwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	hDXGISwapChainDesc.BufferCount = 1;
	hDXGISwapChainDesc.OutputWindow = (HWND)GetNativePtr(0);
	hDXGISwapChainDesc.Windowed = TRUE;
	hDXGISwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	hDXGISwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	if (FAILED(dxgiFactory_->CreateSwapChain(device_, &hDXGISwapChainDesc, &swapChain_)))
	{
		throw "Failed : CreateSwapChain";
	}

	if (FAILED(swapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer_)))
	{
		throw "Failed : GetBuffer";
	}

	if (FAILED(device_->CreateRenderTargetView(backBuffer_, NULL, &renderTargetView_)))
	{
		throw "Failed : CreateRenderTargetView";
	}

	D3D11_TEXTURE2D_DESC hTexture2dDesc;
	hTexture2dDesc.Width = hDXGISwapChainDesc.BufferDesc.Width;
	hTexture2dDesc.Height = hDXGISwapChainDesc.BufferDesc.Height;
	hTexture2dDesc.MipLevels = 1;
	hTexture2dDesc.ArraySize = 1;
	hTexture2dDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	hTexture2dDesc.SampleDesc = hDXGISwapChainDesc.SampleDesc;
	hTexture2dDesc.Usage = D3D11_USAGE_DEFAULT;
	hTexture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	hTexture2dDesc.CPUAccessFlags = 0;
	hTexture2dDesc.MiscFlags = 0;
	if (FAILED(device_->CreateTexture2D(&hTexture2dDesc, NULL, &depthBuffer_)))
	{
		throw "Failed : CreateRenderTargetView";
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC hDepthStencilViewDesc;
	hDepthStencilViewDesc.Format = hTexture2dDesc.Format;
	hDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	hDepthStencilViewDesc.Flags = 0;
	if (FAILED(device_->CreateDepthStencilView(depthBuffer_, &hDepthStencilViewDesc, &depthStencilView_)))
	{
		throw "Failed : CreateDepthStencilView";
	}

	context_->OMSetRenderTargets(1, &renderTargetView_, depthStencilView_);

	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = (float)windowSize[0];
	vp.Height = (float)windowSize[1];
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	context_->RSSetViewports(1, &vp);

	graphicsDevice_ = EffekseerRendererDX11::CreateGraphicsDevice(GetDevice(), GetContext());

	{
		auto gd = static_cast<EffekseerRendererDX11::Backend::GraphicsDevice*>(graphicsDevice_.Get());
		auto rt = gd->CreateTexture(nullptr, GetRenderTargetView(), nullptr);
		auto dt = gd->CreateTexture(nullptr, nullptr, GetDepthStencilView());

		Effekseer::FixedSizeVector<Effekseer::Backend::TextureRef, Effekseer::Backend::RenderTargetMax> rts;
		rts.resize(1);
		rts.at(0) = rt;

		renderPass_ = gd->CreateRenderPass(rts, dt);
	}
}

RenderingEnvironmentDX11 ::~RenderingEnvironmentDX11()
{
	ES_SAFE_RELEASE(renderTargetView_);
	ES_SAFE_RELEASE(backBuffer_);
	ES_SAFE_RELEASE(depthStencilView_);
	ES_SAFE_RELEASE(depthBuffer_);
	ES_SAFE_RELEASE(swapChain_);
	ES_SAFE_RELEASE(dxgiFactory_);
	ES_SAFE_RELEASE(adapter_);
	ES_SAFE_RELEASE(dxgiDevice_);
	ES_SAFE_RELEASE(context_);
	ES_SAFE_RELEASE(device_);
}

void RenderingEnvironmentDX11::Present()
{
	swapChain_->Present(1, 0);
}

bool RenderingEnvironmentDX11::DoEvent()
{
	if (!RenderingEnvironment::DoEvent())
	{
		return false;
	}

	return true;
}
