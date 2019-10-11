#include "EffectPlatformDX11.h"

#include "../../3rdParty/stb/stb_image_write.h"

void EffectPlatformDX11::CreateCheckedTexture()
{
	D3D11_TEXTURE2D_DESC desc;
	desc.ArraySize = 1;
	desc.BindFlags = 0;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.Width = 1280;
	desc.Height = 720;
	desc.MipLevels = 1;
	desc.MiscFlags = 0;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_STAGING;

	auto hr = device_->CreateTexture2D(&desc, 0, &checkedTexture_);
	if (FAILED(hr))
	{
		throw "Failed : CreateTexture";
	}

	D3D11_MAPPED_SUBRESOURCE mr;
	UINT sr = D3D11CalcSubresource(0, 0, 0);
	hr = context_->Map(checkedTexture_, sr, D3D11_MAP_READ_WRITE, 0, &mr);

	std::vector<uint8_t> data;

	data.resize(1280 * 720 * 4);

	for (int32_t h = 0; h < 720; h++)
	{
		auto src_ = &(checkeredPattern_[h * 1280]);
		auto dst_ = &(((uint8_t*)mr.pData)[h * mr.RowPitch]);
		memcpy(dst_, src_, 1280 * 4);
	}

	context_->Unmap(checkedTexture_, sr);
}

EffekseerRenderer::Renderer* EffectPlatformDX11::CreateRenderer()
{
	return EffekseerRendererDX11::Renderer::Create(device_, context_, 2000);
}

EffectPlatformDX11::~EffectPlatformDX11()
{
	ES_SAFE_RELEASE(checkedTexture_);
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

void EffectPlatformDX11::InitializeDevice(const EffectPlatformInitializingParameter& param)
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
	hDXGISwapChainDesc.BufferDesc.Width = 1280;
	hDXGISwapChainDesc.BufferDesc.Height = 720;
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
	vp.Width = (float)1280;
	vp.Height = (float)720;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	context_->RSSetViewports(1, &vp);

	CreateCheckedTexture();
}

void EffectPlatformDX11::BeginRendering()
{
	float ClearColor[] = {0.0f, 0.0f, 0.0f, 1.0f};
	context_->ClearRenderTargetView(renderTargetView_, ClearColor);
	context_->ClearDepthStencilView(depthStencilView_, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	context_->CopyResource(backBuffer_, checkedTexture_);
}

void EffectPlatformDX11::EndRendering() {}

void EffectPlatformDX11::Present() { swapChain_->Present(1, 0); }

bool EffectPlatformDX11::TakeScreenshot(const char* path)
{

	ID3D11Texture2D* cpuTexture = nullptr;

	HRESULT hr;

	D3D11_TEXTURE2D_DESC desc;
	desc.ArraySize = 1;
	desc.BindFlags = 0;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.Width = 1280;
	desc.Height = 720;
	desc.MipLevels = 1;
	desc.MiscFlags = 0;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_STAGING;

	hr = device_->CreateTexture2D(&desc, 0, &cpuTexture);
	if (FAILED(hr))
	{
		throw "Failed : CreateTexture";
	}

	context_->CopyResource(cpuTexture, backBuffer_);

	D3D11_MAPPED_SUBRESOURCE mr;
	UINT sr = D3D11CalcSubresource(0, 0, 0);
	hr = context_->Map(cpuTexture, sr, D3D11_MAP_READ_WRITE, 0, &mr);

	std::vector<uint8_t> data;

	data.resize(1280 * 720 * 4);

	for (int32_t h = 0; h < 720; h++)
	{
		auto dst_ = &(data[h * 1280 * 4]);
		auto src_ = &(((uint8_t*)mr.pData)[h * mr.RowPitch]);
		memcpy(dst_, src_, 1280 * 4);
	}

	context_->Unmap(cpuTexture, sr);

	cpuTexture->Release();

	// HACK for intel
	for (int32_t i = 0; i < 1280 * 720; i++)
	{
		data[i * 4 + 3] = 255;
	}


	stbi_write_png(path, 1280, 720, 4, data.data(), 1280 * 4);

	return true;
}