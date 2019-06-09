
#include "efk.GraphicsDX11.h"

#include <iostream>
#include <fstream>

namespace efk
{
	RenderTextureDX11::RenderTextureDX11(Graphics* graphics)
		: graphics(graphics)
	{
	}

	RenderTextureDX11::~RenderTextureDX11()
	{
		ES_SAFE_RELEASE(texture);
		ES_SAFE_RELEASE(textureSRV);
		ES_SAFE_RELEASE(textureRTV);
	}

	bool RenderTextureDX11::Initialize(int32_t width, int32_t height, TextureFormat format, uint32_t multisample)
	{
		auto g = (GraphicsDX11*)graphics;
		auto r = (EffekseerRendererDX11::Renderer*)g->GetRenderer();

		HRESULT hr;

		D3D11_TEXTURE2D_DESC TexDesc;
		TexDesc.Width = width;
		TexDesc.Height = height;
		TexDesc.MipLevels = 1;
		TexDesc.ArraySize = 1;

		switch (format)
		{
		case TextureFormat::RGBA8U:
			TexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			break;
		case TextureFormat::RGBA16F:
			TexDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
			break;
		case TextureFormat::R16F:
			TexDesc.Format = DXGI_FORMAT_R16_FLOAT;
			break;
		default:
			assert(0);
			return false;
		}

		uint32_t quality = 0;
		r->GetDevice()->CheckMultisampleQualityLevels(TexDesc.Format, multisample, &quality);
		
		TexDesc.SampleDesc.Count = multisample;
		TexDesc.SampleDesc.Quality = quality - 1;
		TexDesc.Usage = D3D11_USAGE_DEFAULT;
		TexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		TexDesc.CPUAccessFlags = 0;
		TexDesc.MiscFlags = 0;

		hr = r->GetDevice()->CreateTexture2D(&TexDesc, nullptr, &texture);
		if (FAILED(hr))
		{
			goto End;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = TexDesc.Format;
		desc.ViewDimension = (multisample > 1) ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MostDetailedMip = 0;
		desc.Texture2D.MipLevels = TexDesc.MipLevels;

		hr = r->GetDevice()->CreateShaderResourceView(texture, &desc, &textureSRV);
		if (FAILED(hr))
		{
			goto End;
		}

		hr = r->GetDevice()->CreateRenderTargetView(texture, NULL, &textureRTV);
		if (FAILED(hr))
		{
			goto End;
		}

		this->width = width;
		this->height = height;
		
		return true;

	End:;
		ES_SAFE_RELEASE(texture);
		ES_SAFE_RELEASE(textureSRV);
		ES_SAFE_RELEASE(textureRTV);
		return false;
	}

	DepthTextureDX11::DepthTextureDX11(Graphics* graphics)
		: graphics(graphics)
	{
	}

	DepthTextureDX11::~DepthTextureDX11()
	{
		ES_SAFE_RELEASE(depthBuffer);
		ES_SAFE_RELEASE(depthStencilView);
		ES_SAFE_RELEASE(depthSRV);
	}

	bool DepthTextureDX11::Initialize(int32_t width, int32_t height, uint32_t multisample)
	{
		auto g = (GraphicsDX11*)graphics;
		auto r = (EffekseerRendererDX11::Renderer*)g->GetRenderer();

		D3D11_TEXTURE2D_DESC desc;
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R24G8_TYPELESS;

		uint32_t quality = 0;
		r->GetDevice()->CheckMultisampleQualityLevels(desc.Format, multisample, &quality);

		desc.SampleDesc.Count = multisample;
		desc.SampleDesc.Quality = quality - 1;

		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		if (FAILED(r->GetDevice()->CreateTexture2D(&desc, NULL, &depthBuffer)))
		{
			goto End;
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc;
		viewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		viewDesc.Flags = 0;
		if (FAILED(r->GetDevice()->CreateDepthStencilView(depthBuffer, &viewDesc, &depthStencilView)))
		{
			goto End;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(srvDesc));
		srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		srvDesc.ViewDimension = (multisample > 1) ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = desc.MipLevels;

		if (FAILED(r->GetDevice()->CreateShaderResourceView(depthBuffer, &srvDesc, &depthSRV)))
		{
			goto End;
		}

		this->width = width;
		this->height = height;

		return true;

	End:;
		ES_SAFE_RELEASE(depthBuffer);
		ES_SAFE_RELEASE(depthStencilView);
		ES_SAFE_RELEASE(depthSRV);
		return false;
	}

	GraphicsDX11::GraphicsDX11()
	{

	}

	GraphicsDX11::~GraphicsDX11()
	{
		if (renderer != nullptr)
		{
			renderer->Destroy();
			renderer = nullptr;
		}

		//assert(renderDefaultTarget == nullptr);
		//assert(recordingTarget == nullptr);
		//
		
		ES_SAFE_RELEASE(backTexture);
		ES_SAFE_RELEASE(backTextureSRV);

		ES_SAFE_RELEASE(defaultRenderTarget);
		ES_SAFE_RELEASE(defaultDepthStencil);
		ES_SAFE_RELEASE(renderTargetView);
		ES_SAFE_RELEASE(depthStencilView);

		ES_SAFE_RELEASE(swapChain);
		ES_SAFE_RELEASE(dxgiFactory);
		ES_SAFE_RELEASE(adapter);
		ES_SAFE_RELEASE(dxgiDevice);
		ES_SAFE_RELEASE(context);
		ES_SAFE_RELEASE(device);

		ES_SAFE_RELEASE(rasterizerState);
		ES_SAFE_RELEASE(savedRasterizerState);

		if (d3dDebug != nullptr)
		{
			//d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		}
		ES_SAFE_RELEASE(d3dDebug);
	}

	bool GraphicsDX11::Initialize(void* windowHandle, int32_t windowWidth, int32_t windowHeight, bool isSRGBMode, int32_t spriteCount)
	{
		std::string log = "";

		UINT debugFlag = 0;
#if _DEBUG
		debugFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif
		D3D_FEATURE_LEVEL flevels[] = {
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
		};
		int32_t flevelCount = sizeof(flevels) / sizeof(D3D_FEATURE_LEVEL);

		D3D_FEATURE_LEVEL currentFeatureLevel;

		HRESULT hr = D3D11CreateDevice(
			NULL,
			D3D_DRIVER_TYPE_HARDWARE,
			NULL,
			debugFlag,
			flevels,
			flevelCount,
			D3D11_SDK_VERSION,
			&device,
			NULL,
			&context);

		if FAILED(hr)
		{
			log += "Failed : D3D11CreateDevice\n";
			goto End;
		}

		#if DEBUG
		if (FAILED( device->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&d3dDebug))))
		{
			log += "Failed : QueryInterface(Debug)\n";
			goto End;
		}
		#endif

		if (FAILED(device->QueryInterface(__uuidof(IDXGIDevice1), (void**)&dxgiDevice)))
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
		if (dxgiFactory == NULL)
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

		if (FAILED(dxgiFactory->CreateSwapChain(device, &hDXGISwapChainDesc, &swapChain)))
		{
			log += "Failed : CreateSwapChain\n";
			goto End;
		}

		if (FAILED(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&defaultRenderTarget)))
		{
			log += "Failed : GetBuffer\n";
			goto End;
		}

		if (FAILED(device->CreateRenderTargetView(defaultRenderTarget, NULL, &renderTargetView)))
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
		if (FAILED(device->CreateTexture2D(&hTexture2dDesc, NULL, &defaultDepthStencil)))
		{
			log += "Failed : CreateTexture2D\n";
			goto End;
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC hDepthStencilViewDesc;
		hDepthStencilViewDesc.Format = hTexture2dDesc.Format;
		hDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		hDepthStencilViewDesc.Flags = 0;
		if (FAILED(device->CreateDepthStencilView(defaultDepthStencil, &hDepthStencilViewDesc, &depthStencilView)))
		{
			log += "Failed : CreateDepthStencilView\n";
			goto End;
		}

		context->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
		currentRenderTargetView = renderTargetView;
		currentDepthStencilView = depthStencilView;

		this->isSRGBMode = isSRGBMode;
		this->windowHandle = windowHandle;
		this->windowWidth = windowWidth;
		this->windowHeight = windowHeight;

		renderer = ::EffekseerRendererDX11::Renderer::Create(device, context, spriteCount);

		D3D11_RASTERIZER_DESC rasterizerDesc;
		ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		rasterizerDesc.ScissorEnable = false;
		rasterizerDesc.DepthClipEnable = false;
		rasterizerDesc.MultisampleEnable = true;
		device->CreateRasterizerState(&rasterizerDesc, &rasterizerState);

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
		ES_SAFE_RELEASE(device);


		std::ofstream outputfile("error_native.txt");
		outputfile << log.c_str();
		outputfile.close();

		return false;
	}

	void GraphicsDX11::CopyToBackground()
	{
		HRESULT hr = S_OK;
		ID3D11RenderTargetView* renderTargetView = nullptr;
		ID3D11Texture2D* renderTexture = nullptr;
		context->OMGetRenderTargets(1, &renderTargetView, nullptr);
		renderTargetView->GetResource(reinterpret_cast<ID3D11Resource**>(&renderTexture));
		
		// Get rendertarget infromation
		D3D11_TEXTURE2D_DESC renderTextureDesc;
		renderTexture->GetDesc(&renderTextureDesc);

		D3D11_TEXTURE2D_DESC backGroundTextureDesc;
		ZeroMemory(&backGroundTextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
		if (backTexture != nullptr)
		{
			backTexture->GetDesc(&backGroundTextureDesc);
		}

		// Get scissor
		UINT numScissorRects = 1;
		D3D11_RECT scissorRect;
		context->RSGetScissorRects(&numScissorRects, &scissorRect);
		
		// Calculate area to draw
		uint32_t width = renderTextureDesc.Width;
		uint32_t height = renderTextureDesc.Height;
		if (numScissorRects > 0)
		{
			width = scissorRect.right - scissorRect.left;
			height = scissorRect.bottom - scissorRect.top;
		}

		// if size is changed, rebuild it
		if (backTexture == nullptr ||
			backGroundTextureDesc.Width != width ||
			backGroundTextureDesc.Height != height ||
			backGroundTextureDesc.Format != renderTextureDesc.Format)
		{
			ES_SAFE_RELEASE(backTexture);
			ES_SAFE_RELEASE(backTextureSRV);

			ZeroMemory(&backGroundTextureDesc, sizeof(backGroundTextureDesc));
			backGroundTextureDesc.Usage = D3D11_USAGE_DEFAULT;
			backGroundTextureDesc.Format = renderTextureDesc.Format;
			backGroundTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			backGroundTextureDesc.Width = width;
			backGroundTextureDesc.Height = height;
			backGroundTextureDesc.CPUAccessFlags = 0;
			backGroundTextureDesc.MipLevels = 1;
			backGroundTextureDesc.ArraySize = 1;
			backGroundTextureDesc.SampleDesc.Count = 1;
			backGroundTextureDesc.SampleDesc.Quality = 0;

			HRESULT hr = S_OK;
			hr = device->CreateTexture2D(&backGroundTextureDesc, nullptr, &backTexture);
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			ZeroMemory(&srvDesc, sizeof(srvDesc));
			srvDesc.Format = renderTextureDesc.Format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = 1;
			hr = device->CreateShaderResourceView(backTexture, &srvDesc, &backTextureSRV);
		}


		if (width == renderTextureDesc.Width &&
			height == renderTextureDesc.Height)
		{
			if (renderTextureDesc.SampleDesc.Count == backGroundTextureDesc.SampleDesc.Count)
			{
				// Copy to background
				context->CopyResource(backTexture, renderTexture);
			}
			else
			{
				context->ResolveSubresource(backTexture, 0,
					renderTexture, 0, backGroundTextureDesc.Format);
			}
		}
		else
		{
			D3D11_BOX srcBox;
			srcBox.left = scissorRect.left;
			srcBox.top = scissorRect.top;
			srcBox.right = scissorRect.right;
			srcBox.bottom = scissorRect.bottom;
			srcBox.front = 0;
			srcBox.back = 1;
			context->CopySubresourceRegion(backTexture, 0, 0, 0, 0, renderTexture, 0, &srcBox);
		}

		ES_SAFE_RELEASE(renderTexture);
		ES_SAFE_RELEASE(renderTargetView);
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
		if (isSRGBMode)
		{
		}

		assert(savedRasterizerState == nullptr);
		context->RSGetState(&savedRasterizerState);
		context->RSSetState(rasterizerState);
	}

	void GraphicsDX11::EndScene()
	{
		context->RSSetState(savedRasterizerState);
		ES_SAFE_RELEASE(savedRasterizerState);

		if (isSRGBMode)
		{
		}
	}

	void GraphicsDX11::SetRenderTarget(RenderTexture* renderTexture, DepthTexture* depthTexture)
	{
		currentRenderTexture = renderTexture;
		currentDepthTexture = depthTexture;

		auto rt = (RenderTextureDX11*)renderTexture;
		auto dt = (DepthTextureDX11*)depthTexture;

		if (renderTexture == nullptr)
		{
			context->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
			currentRenderTargetView = renderTargetView;
			currentDepthStencilView = depthStencilView;

		}
		else
		{
			ID3D11RenderTargetView*	renderTargetView_ = nullptr;
			ID3D11DepthStencilView*	depthStencilView_ = nullptr;

			if (rt != nullptr)
			{
				renderTargetView_ = rt->GetRenderTargetView();
			}
			
			if (dt != nullptr)
			{
				depthStencilView_ = dt->GetDepthStencilView();
			}

			context->OMSetRenderTargets(1, &renderTargetView_, depthStencilView_);

			D3D11_VIEWPORT vp;
			vp.TopLeftX = 0;
			vp.TopLeftY = 0;
			vp.Width = (float)renderTexture->GetWidth();
			vp.Height = (float)renderTexture->GetHeight();
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			context->RSSetViewports(1, &vp);

			currentRenderTargetView = renderTargetView_;
			currentDepthStencilView = depthStencilView_;

		}
	}

	void GraphicsDX11::BeginRecord(int32_t width, int32_t height)
	{
		HRESULT hr;

		backupRenderTarget = currentRenderTexture;
		backupDepthStencil = currentDepthTexture;

		recordingTexture = RenderTextureDX11::Create(this);
		recordingTexture->Initialize(width, height, TextureFormat::RGBA8U);

		recordingDepthStencil = DepthTextureDX11::Create(this);
		recordingDepthStencil->Initialize(width, height);

		SetRenderTarget(recordingTexture, recordingDepthStencil);
		recordingWidth = width;
		recordingHeight = height;

		D3D11_VIEWPORT vp;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		vp.Width = (float)width;
		vp.Height = (float)height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		context->RSSetViewports(1, &vp);
	}

	void GraphicsDX11::EndRecord(std::vector<Effekseer::Color>& pixels)
	{
		HRESULT hr;

		SetRenderTarget(backupRenderTarget, backupDepthStencil);
		backupRenderTarget = nullptr;
		backupDepthStencil = nullptr;

		ID3D11Texture2D* texture_ = nullptr;

		D3D11_TEXTURE2D_DESC recordingTextureDesc;
		ZeroMemory(&recordingTextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
		if (recordingTexture != nullptr)
		{
			((RenderTextureDX11*)recordingTexture)->GetTexture()->GetDesc(&recordingTextureDesc);
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

		device->CreateTexture2D(&desc, 0, &texture_);

		context->CopyResource(texture_, ((RenderTextureDX11*)recordingTexture)->GetTexture());

		D3D11_MAPPED_SUBRESOURCE mr;
		UINT sr = D3D11CalcSubresource(0, 0, 0);
		hr = context->Map(texture_, sr, D3D11_MAP_READ_WRITE, 0, &mr);
		
		assert(SUCCEEDED(hr));

		pixels.resize(recordingTextureDesc.Width * recordingTextureDesc.Height);

		for (int32_t h = 0; h < recordingTextureDesc.Height; h++)
		{
			auto dst_ = &(pixels[h * recordingTextureDesc.Width]);
			auto src_ = &(((uint8_t*)mr.pData)[h*mr.RowPitch]);
			memcpy(dst_, src_, recordingTextureDesc.Width * sizeof(Effekseer::Color));
		}

		context->Unmap(texture_, sr);

		ES_SAFE_RELEASE(texture_);
		ES_SAFE_DELETE(recordingTexture);
		ES_SAFE_DELETE(recordingDepthStencil);
	}

	void GraphicsDX11::Clear(Effekseer::Color color)
	{
		float ClearColor[] = { color.R / 255.0f, color.G / 255.0f, color.B / 255.0f, color.A / 255.0f };
		context->ClearRenderTargetView(currentRenderTargetView, ClearColor);
		context->ClearDepthStencilView(currentDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	void GraphicsDX11::ResolveRenderTarget(RenderTexture* src, RenderTexture* dest)
	{
		auto srcDX11 = (RenderTextureDX11*)src;
		auto destDX11 = (RenderTextureDX11*)dest;

		context->ResolveSubresource(
			destDX11->GetTexture(), 0,
			srcDX11->GetTexture(), 0,
			DXGI_FORMAT_R16G16B16A16_FLOAT);
	}

	void GraphicsDX11::ResetDevice()
	{
		renderer->OnLostDevice();

		if (LostedDevice != nullptr)
		{
			LostedDevice();
		}

		ES_SAFE_RELEASE(defaultRenderTarget);
		ES_SAFE_RELEASE(defaultDepthStencil);
		ES_SAFE_RELEASE(renderTargetView);
		ES_SAFE_RELEASE(depthStencilView);

		swapChain->ResizeBuffers(1, windowWidth, windowHeight, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

		if (FAILED(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&defaultRenderTarget)))
		{
			return;
		}

		if (FAILED(device->CreateRenderTargetView(defaultRenderTarget, NULL, &renderTargetView)))
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
		if (FAILED(device->CreateTexture2D(&hTexture2dDesc, NULL, &defaultDepthStencil)))
		{
			return;
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC hDepthStencilViewDesc;
		hDepthStencilViewDesc.Format = hTexture2dDesc.Format;
		hDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		hDepthStencilViewDesc.Flags = 0;
		if (FAILED(device->CreateDepthStencilView(defaultDepthStencil, &hDepthStencilViewDesc, &depthStencilView)))
		{
			return;
		}

		context->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

		currentRenderTargetView = renderTargetView;
		currentDepthStencilView = depthStencilView;

		if (ResettedDevice != nullptr)
		{
			ResettedDevice();
		}

		renderer->OnResetDevice();
	}

	void* GraphicsDX11::GetBack()
	{
		return backTextureSRV;
	}

	EffekseerRenderer::Renderer* GraphicsDX11::GetRenderer()
	{
		return renderer;
	}
}