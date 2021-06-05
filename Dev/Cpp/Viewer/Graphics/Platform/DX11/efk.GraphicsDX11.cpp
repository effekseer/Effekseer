
#include "efk.GraphicsDX11.h"

#include <fstream>
#include <iostream>

namespace efk
{
RenderTextureDX11::RenderTextureDX11(Graphics* graphics)
	: graphics(graphics)
{
}

RenderTextureDX11::~RenderTextureDX11()
{
}

bool RenderTextureDX11::Initialize(Effekseer::Tool::Vector2DI size, Effekseer::Backend::TextureFormatType format, uint32_t multisample)
{
	auto g = (GraphicsDX11*)graphics;
	auto gd = g->GetGraphicsDevice().DownCast<EffekseerRendererDX11::Backend::GraphicsDevice>();

	Effekseer::Backend::RenderTextureParameter param;
	param.Format = format;
	param.SamplingCount = multisample;
	param.Size = {size.X, size.Y};
	texture_ = gd->CreateRenderTexture(param).DownCast<EffekseerRendererDX11::Backend::Texture>();

	this->size_ = size;
	this->samplingCount_ = multisample;
	this->format_ = format;

	return texture_ != nullptr;
}

DepthTextureDX11::DepthTextureDX11(Graphics* graphics)
	: graphics(graphics)
{
}

DepthTextureDX11::~DepthTextureDX11()
{
}

bool DepthTextureDX11::Initialize(int32_t width, int32_t height, uint32_t multisample)
{
	auto g = (GraphicsDX11*)graphics;
	auto gd = g->GetGraphicsDevice().DownCast<EffekseerRendererDX11::Backend::GraphicsDevice>();

	Effekseer::Backend::DepthTextureParameter param;
	param.Format = Effekseer::Backend::TextureFormatType::D24S8;
	param.SamplingCount = multisample;
	param.Size = {width, height};
	texture_ = gd->CreateDepthTexture(param).DownCast<EffekseerRendererDX11::Backend::Texture>();
	return texture_ != nullptr;
}

GraphicsDX11::GraphicsDX11()
{
}

GraphicsDX11::~GraphicsDX11()
{
	// assert(renderDefaultTarget == nullptr);
	// assert(recordingTarget == nullptr);
	//

	//ES_SAFE_RELEASE(backTexture);
	//ES_SAFE_RELEASE(backTextureSRV);

	ES_SAFE_RELEASE(defaultRenderTarget);
	ES_SAFE_RELEASE(defaultDepthStencil);
	ES_SAFE_RELEASE(renderTargetView);
	ES_SAFE_RELEASE(depthStencilView);

	ES_SAFE_RELEASE(swapChain);
	ES_SAFE_RELEASE(dxgiFactory);
	ES_SAFE_RELEASE(adapter);
	ES_SAFE_RELEASE(dxgiDevice);
	ES_SAFE_RELEASE(context);
	ES_SAFE_RELEASE(device_);

	ES_SAFE_RELEASE(rasterizerState);
	ES_SAFE_RELEASE(savedRasterizerState);

	if (d3dDebug != nullptr)
	{
		// d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	}
	ES_SAFE_RELEASE(d3dDebug);
}

bool GraphicsDX11::Initialize(void* windowHandle, int32_t windowWidth, int32_t windowHeight)
{
	std::string log = "";

	UINT debugFlag = 0;
#if _DEBUG
	debugFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL desigeredFeatureLevels[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	int32_t flevelCount = sizeof(desigeredFeatureLevels) / sizeof(D3D_FEATURE_LEVEL);

	HRESULT hr = D3D11CreateDevice(
		nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, debugFlag, desigeredFeatureLevels, flevelCount, D3D11_SDK_VERSION, &device_, &flevel_, &context);

	if (FAILED(hr))
	{
		log += "Failed : D3D11CreateDevice\n";
		goto End;
	}

#if DEBUG
	if (FAILED(device->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&d3dDebug))))
	{
		log += "Failed : QueryInterface(Debug)\n";
		goto End;
	}
#endif

	if (FAILED(device_->QueryInterface(__uuidof(IDXGIDevice1), (void**)&dxgiDevice)))
	{
		log += "Failed : QueryInterface\n";
		goto End;
	}

	if (FAILED(dxgiDevice->GetAdapter(&adapter)))
	{
		log += "Failed : GetAdapter\n";
		goto End;
	}

	adapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);
	if (dxgiFactory == nullptr)
	{
		log += "Failed : GetParent\n";
		goto End;
	}

	DXGI_SWAP_CHAIN_DESC hDXGISwapChainDesc;
	hDXGISwapChainDesc.BufferCount = 2;
	hDXGISwapChainDesc.BufferDesc.Width = windowWidth;
	hDXGISwapChainDesc.BufferDesc.Height = windowHeight;
	hDXGISwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	hDXGISwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	hDXGISwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	hDXGISwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	hDXGISwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	hDXGISwapChainDesc.SampleDesc.Count = 1;
	hDXGISwapChainDesc.SampleDesc.Quality = 0;
	hDXGISwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	hDXGISwapChainDesc.BufferCount = 1;
	hDXGISwapChainDesc.OutputWindow = (HWND)windowHandle;
	hDXGISwapChainDesc.Windowed = TRUE;
	hDXGISwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	hDXGISwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	if (FAILED(dxgiFactory->CreateSwapChain(device_, &hDXGISwapChainDesc, &swapChain)))
	{
		log += "Failed : CreateSwapChain\n";
		goto End;
	}

	if (FAILED(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&defaultRenderTarget)))
	{
		log += "Failed : GetBuffer\n";
		goto End;
	}

	if (FAILED(device_->CreateRenderTargetView(defaultRenderTarget, nullptr, &renderTargetView)))
	{
		log += "Failed : CreateRenderTargetView\n";
		goto End;
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
	if (FAILED(device_->CreateTexture2D(&hTexture2dDesc, nullptr, &defaultDepthStencil)))
	{
		log += "Failed : CreateTexture2D\n";
		goto End;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC hDepthStencilViewDesc;
	hDepthStencilViewDesc.Format = hTexture2dDesc.Format;
	hDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	hDepthStencilViewDesc.Flags = 0;
	if (FAILED(device_->CreateDepthStencilView(defaultDepthStencil, &hDepthStencilViewDesc, &depthStencilView)))
	{
		log += "Failed : CreateDepthStencilView\n";
		goto End;
	}

	currentRenderTargetViews.fill(nullptr);

	context->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
	currentRenderTargetViews[0] = renderTargetView;
	currentDepthStencilView = depthStencilView;

	this->isSRGBMode = isSRGBMode;
	this->windowHandle = windowHandle;
	this->windowWidth = windowWidth;
	this->windowHeight = windowHeight;

	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.DepthClipEnable = false;
	rasterizerDesc.MultisampleEnable = true;
	device_->CreateRasterizerState(&rasterizerDesc, &rasterizerState);

	graphicsDevice_ = Effekseer::MakeRefPtr<EffekseerRendererDX11::Backend::GraphicsDevice>(device_, context);

	return true;
End:
	ES_SAFE_RELEASE(rasterizerState);
	ES_SAFE_RELEASE(renderTargetView);
	ES_SAFE_RELEASE(defaultRenderTarget);
	ES_SAFE_RELEASE(depthStencilView);
	ES_SAFE_RELEASE(defaultDepthStencil);
	ES_SAFE_RELEASE(swapChain);
	ES_SAFE_RELEASE(dxgiFactory);
	ES_SAFE_RELEASE(adapter);
	ES_SAFE_RELEASE(dxgiDevice);
	ES_SAFE_RELEASE(context);
	ES_SAFE_RELEASE(device_);

	std::ofstream outputfile("error_native.txt");
	outputfile << log.c_str();
	outputfile.close();

	return false;
}

void GraphicsDX11::CopyTo(Effekseer::Backend::TextureRef src, Effekseer::Backend::TextureRef dst)
{
	if (src->GetSize() != dst->GetSize())
		return;

	if (src->GetFormat() != dst->GetFormat())
		return;

	if (src->GetSamplingCount() != dst->GetSamplingCount())
	{
		ResolveRenderTarget(src, dst);
	}
	else
	{
		// Copy to background
		auto s = src.DownCast<EffekseerRendererDX11::Backend::Texture>();
		auto d = dst.DownCast<EffekseerRendererDX11::Backend::Texture>();
		context->CopyResource(d->GetTexture(), s->GetTexture());
	}
}

void GraphicsDX11::Resize(int32_t width, int32_t height)
{
	this->windowWidth = width;
	this->windowHeight = height;
	ResetDevice();
}

bool GraphicsDX11::Present()
{
	swapChain->Present(1, 0);

	if (Presented != nullptr)
	{
		Presented();
	}

	return true;
}

void GraphicsDX11::BeginScene()
{
	assert(savedRasterizerState == nullptr);
	context->RSGetState(&savedRasterizerState);
	context->RSSetState(rasterizerState);
}

void GraphicsDX11::EndScene()
{
	context->RSSetState(savedRasterizerState);
	ES_SAFE_RELEASE(savedRasterizerState);
}

void GraphicsDX11::SetRenderTarget(std::vector<Effekseer::Backend::TextureRef> renderTextures, Effekseer::Backend::TextureRef depthTexture)
{
	assert(renderTextures.size() > 0);

	if (renderTextures[0] == nullptr)
	{
		currentRenderTargetViews.fill(nullptr);
		context->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
		currentRenderTargetViews[0] = renderTargetView;
		currentDepthStencilView = depthStencilView;
	}
	else
	{
		std::array<ID3D11RenderTargetView*, 4> renderTargetView_;
		renderTargetView_.fill(nullptr);

		auto dt = depthTexture.DownCast<EffekseerRendererDX11::Backend::Texture>();

		ID3D11DepthStencilView* depthStencilView_ = nullptr;

		for (size_t i = 0; i < renderTextures.size(); i++)
		{
			auto rt = renderTextures[i].DownCast<EffekseerRendererDX11::Backend::Texture>();
			if (rt != nullptr)
			{
				renderTargetView_[i] = rt->GetRTV();
			}
		}

		if (dt != nullptr)
		{
			depthStencilView_ = dt->GetDSV();
		}

		context->OMSetRenderTargets(renderTextures.size(), renderTargetView_.data(), depthStencilView_);

		std::array<D3D11_VIEWPORT, 4> vps;

		for (auto& vp : vps)
		{
			vp.TopLeftX = 0;
			vp.TopLeftY = 0;
			vp.Width = static_cast<float>(renderTextures[0]->GetSize()[0]);
			vp.Height = static_cast<float>(renderTextures[0]->GetSize()[1]);
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
		}

		context->RSSetViewports(renderTextures.size(), vps.data());

		currentRenderTargetViews = renderTargetView_;
		currentDepthStencilView = depthStencilView_;
	}
}

void GraphicsDX11::SaveTexture(Effekseer::Backend::TextureRef texture, std::vector<Effekseer::Color>& pixels)
{
	auto t = texture.DownCast<EffekseerRendererDX11::Backend::Texture>();

	HRESULT hr;

	ID3D11Texture2D* texture_ = nullptr;

	D3D11_TEXTURE2D_DESC recordingTextureDesc;
	ZeroMemory(&recordingTextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
	if (t != nullptr)
	{
		t->GetTexture()->GetDesc(&recordingTextureDesc);
	}

	D3D11_TEXTURE2D_DESC desc;
	desc.ArraySize = 1;
	desc.BindFlags = 0;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.Width = recordingTextureDesc.Width;
	desc.Height = recordingTextureDesc.Height;
	desc.MipLevels = 1;
	desc.MiscFlags = 0;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_STAGING;

	device_->CreateTexture2D(&desc, 0, &texture_);

	context->CopyResource(texture_, t->GetTexture());

	D3D11_MAPPED_SUBRESOURCE mr;
	UINT sr = D3D11CalcSubresource(0, 0, 0);
	hr = context->Map(texture_, sr, D3D11_MAP_READ_WRITE, 0, &mr);

	assert(SUCCEEDED(hr));

	pixels.resize(recordingTextureDesc.Width * recordingTextureDesc.Height);

	for (UINT h = 0; h < recordingTextureDesc.Height; h++)
	{
		auto dst_ = &(pixels[h * recordingTextureDesc.Width]);
		auto src_ = &(((uint8_t*)mr.pData)[h * mr.RowPitch]);
		memcpy(dst_, src_, recordingTextureDesc.Width * sizeof(Effekseer::Color));
	}

	context->Unmap(texture_, sr);

	ES_SAFE_RELEASE(texture_);
}

void GraphicsDX11::Clear(Effekseer::Color color)
{
	float ClearColor[] = {color.R / 255.0f, color.G / 255.0f, color.B / 255.0f, color.A / 255.0f};

	for (size_t i = 0; i < currentRenderTargetViews.size(); i++)
	{
		if (currentRenderTargetViews[i] == nullptr)
			continue;

		context->ClearRenderTargetView(currentRenderTargetViews[i], ClearColor);
	}

	if (currentDepthStencilView != nullptr)
	{
		context->ClearDepthStencilView(currentDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}
}

void GraphicsDX11::ResolveRenderTarget(Effekseer::Backend::TextureRef src, Effekseer::Backend::TextureRef dest)
{
	auto srcDX11 = src.DownCast<EffekseerRendererDX11::Backend::Texture>();
	auto destDX11 = dest.DownCast<EffekseerRendererDX11::Backend::Texture>();

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	srcDX11->GetTexture()->GetDesc(&desc);

	context->ResolveSubresource(
		destDX11->GetTexture(), 0, srcDX11->GetTexture(), 0, desc.Format);
}

bool GraphicsDX11::CheckFormatSupport(Effekseer::Backend::TextureFormatType format, TextureFeatureType feature)
{
	const DXGI_FORMAT dxgiformat = EffekseerRendererDX11::Backend::GetTextureFormatType(format);

	UINT dxfeature = 0;
	device_->CheckFormatSupport(dxgiformat, &dxfeature);

	if (feature == TextureFeatureType::Texture2D)
	{
		return dxfeature & D3D11_FORMAT_SUPPORT_TEXTURE2D;
	}
	else if (feature == TextureFeatureType::MultisampledTexture2DRenderTarget)
	{
		return dxfeature & D3D11_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET;
	}
	else if (feature == TextureFeatureType::MultisampledTexture2DResolve)
	{
		return dxfeature & D3D11_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE;
	}
	else
	{
		assert(0);
	}

	return false;
}

int GraphicsDX11::GetMultisampleLevel(Effekseer::Backend::TextureFormatType format)
{
	// old video cards cannot be debugged on many contributor's environment
	if (flevel_ != D3D_FEATURE_LEVEL_11_0)
	{
		return 1;
	}

	const DXGI_FORMAT dxgiformat = EffekseerRendererDX11::Backend::GetTextureFormatType(format);

	for (int i = 1; i <= D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT; i <<= 1)
	{
		UINT Quality;
		if (SUCCEEDED(device_->CheckMultisampleQualityLevels(dxgiformat, i, &Quality)))
		{
			if (0 == Quality)
			{
				return i >> 1;
			}
		}
	}

	return 1;
}

void GraphicsDX11::ResetDevice()
{
	ES_SAFE_RELEASE(defaultRenderTarget);
	ES_SAFE_RELEASE(defaultDepthStencil);
	ES_SAFE_RELEASE(renderTargetView);
	ES_SAFE_RELEASE(depthStencilView);

	swapChain->ResizeBuffers(1, windowWidth, windowHeight, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

	if (FAILED(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&defaultRenderTarget)))
	{
		return;
	}

	if (FAILED(device_->CreateRenderTargetView(defaultRenderTarget, nullptr, &renderTargetView)))
	{
		return;
	}

	D3D11_TEXTURE2D_DESC hTexture2dDesc;
	hTexture2dDesc.Width = windowWidth;
	hTexture2dDesc.Height = windowHeight;
	hTexture2dDesc.MipLevels = 1;
	hTexture2dDesc.ArraySize = 1;
	hTexture2dDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	hTexture2dDesc.SampleDesc.Count = 1;
	hTexture2dDesc.SampleDesc.Quality = 0;
	hTexture2dDesc.Usage = D3D11_USAGE_DEFAULT;
	hTexture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	hTexture2dDesc.CPUAccessFlags = 0;
	hTexture2dDesc.MiscFlags = 0;
	if (FAILED(device_->CreateTexture2D(&hTexture2dDesc, nullptr, &defaultDepthStencil)))
	{
		return;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC hDepthStencilViewDesc;
	hDepthStencilViewDesc.Format = hTexture2dDesc.Format;
	hDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	hDepthStencilViewDesc.Flags = 0;
	if (FAILED(device_->CreateDepthStencilView(defaultDepthStencil, &hDepthStencilViewDesc, &depthStencilView)))
	{
		return;
	}

	context->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

	currentRenderTargetViews.fill(nullptr);
	currentRenderTargetViews[0] = renderTargetView;
	currentDepthStencilView = depthStencilView;
}

ID3D11Device* GraphicsDX11::GetDevice() const
{
	return device_;
}

ID3D11DeviceContext* GraphicsDX11::GetContext() const
{
	return context;
}

} // namespace efk