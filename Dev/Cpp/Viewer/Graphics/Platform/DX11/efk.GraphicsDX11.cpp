
#include "efk.GraphicsDX11.h"

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

	bool RenderTextureDX11::Initialize(int32_t width, int32_t height)
	{
		auto g = (GraphicsDX11*)graphics;
		auto r = (EffekseerRendererDX11::Renderer*)g->GetRenderer();

		HRESULT hr;

		D3D11_TEXTURE2D_DESC TexDesc;
		TexDesc.Width = width;
		TexDesc.Height = height;
		TexDesc.MipLevels = 1;
		TexDesc.ArraySize = 1;
		TexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		TexDesc.SampleDesc.Count = 1;
		TexDesc.SampleDesc.Quality = 0;
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
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
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

	bool DepthTextureDX11::Initialize(int32_t width, int32_t height)
	{
		auto g = (GraphicsDX11*)graphics;
		auto r = (EffekseerRendererDX11::Renderer*)g->GetRenderer();

		D3D11_TEXTURE2D_DESC desc;
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
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
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
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
	}

	bool GraphicsDX11::Initialize(void* windowHandle, int32_t windowWidth, int32_t windowHeight, bool isSRGBMode, int32_t spriteCount)
	{
		UINT debugFlag = 0;
		debugFlag = D3D11_CREATE_DEVICE_DEBUG;

		D3D_FEATURE_LEVEL flevels[] = {
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1,
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
			goto End;
		}

		if (FAILED(device->QueryInterface(__uuidof(IDXGIDevice1), (void**)&dxgiDevice)))
		{
			goto End;
		}

		if (FAILED(dxgiDevice->GetAdapter(&adapter)))
		{
			goto End;
		}

		adapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);
		if (dxgiFactory == NULL)
		{
			goto End;
		}

		DXGI_SWAP_CHAIN_DESC hDXGISwapChainDesc;
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
			goto End;
		}

		if (FAILED(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&defaultRenderTarget)))
		{
			goto End;
		}

		if (FAILED(device->CreateRenderTargetView(defaultRenderTarget, NULL, &renderTargetView)))
		{
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
			goto End;
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC hDepthStencilViewDesc;
		hDepthStencilViewDesc.Format = hTexture2dDesc.Format;
		hDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		hDepthStencilViewDesc.Flags = 0;
		if (FAILED(device->CreateDepthStencilView(defaultDepthStencil, &hDepthStencilViewDesc, &depthStencilView)))
		{
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

		return true;
	End:
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
			srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = 1;
			hr = device->CreateShaderResourceView(backTexture, &srvDesc, &backTextureSRV);
		}
		if (width == renderTextureDesc.Width &&
			height == renderTextureDesc.Height)
		{
			// Copy to background
			context->CopyResource(backTexture, renderTexture);
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
	}

	void GraphicsDX11::EndScene()
	{
		if (isSRGBMode)
		{
		}
	}

	void GraphicsDX11::SetRenderTarget(RenderTexture* renderTexture, DepthTexture* depthTexture)
	{
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
		/*
		HRESULT hr;

		hr = d3d_device->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &recordingTargetTexture, NULL);
		if (FAILED(hr)) return;

		d3d_device->GetRenderTarget(0, &renderDefaultTarget);
		d3d_device->GetDepthStencilSurface(&renderDefaultDepth);

		recordingTargetTexture->GetSurfaceLevel(0, &recordingTarget);

		d3d_device->CreateDepthStencilSurface(width, height, D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, TRUE, &recordingDepth, NULL);

		recordingWidth = width;
		recordingHeight = height;

		d3d_device->SetRenderTarget(0, recordingTarget);
		d3d_device->SetDepthStencilSurface(recordingDepth);
		*/
	}

	void GraphicsDX11::EndRecord(std::vector<Effekseer::Color>& pixels)
	{
		/*
		pixels.resize(recordingWidth * recordingHeight);

		d3d_device->SetRenderTarget(0, renderDefaultTarget);
		d3d_device->SetDepthStencilSurface(renderDefaultDepth);
		ES_SAFE_RELEASE(renderDefaultTarget);
		ES_SAFE_RELEASE(renderDefaultDepth);

		IDirect3DSurface9*	temp_sur = nullptr;
		
		d3d_device->CreateOffscreenPlainSurface(
			recordingWidth, recordingHeight, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &temp_sur, NULL);
		
		d3d_device->GetRenderTargetData(recordingTarget, temp_sur);

		D3DLOCKED_RECT drect;
		RECT rect;
		rect.top = 0;
		rect.left = 0;
		rect.right = recordingWidth;
		rect.bottom = recordingHeight;

		auto hr = temp_sur->LockRect(&drect, &rect, D3DLOCK_READONLY);
		if (SUCCEEDED(hr))
		{
			for (int32_t y = 0; y < recordingHeight; y++)
			{
				for (int32_t x = 0; x < recordingWidth; x++)
				{
					auto src = &(((uint8_t*)drect.pBits)[x * 4 + drect.Pitch * y]);
					pixels[x + recordingWidth * y].A = src[3];
					pixels[x + recordingWidth * y].R = src[2];
					pixels[x + recordingWidth * y].G = src[1];
					pixels[x + recordingWidth * y].B = src[0];
				}
			}

			temp_sur->UnlockRect();
		}

		ES_SAFE_RELEASE(temp_sur);

		ES_SAFE_RELEASE(recordingTarget);
		ES_SAFE_RELEASE(recordingTargetTexture);
		ES_SAFE_RELEASE(recordingDepth);
		*/
	}

	void GraphicsDX11::Clear(Effekseer::Color color)
	{
		float ClearColor[] = { color.R / 255.0f, color.G / 255.0f, color.B / 255.0f, color.A / 255.0f };
		context->ClearRenderTargetView(currentRenderTargetView, ClearColor);
		context->ClearDepthStencilView(currentDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
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

		if (ResettedDevice != nullptr)
		{
			ResettedDevice();
		}

		renderer->OnResetDevice();
	}

	void* GraphicsDX11::GetBack()
	{
		return backTexture;
	}

	EffekseerRenderer::Renderer* GraphicsDX11::GetRenderer()
	{
		return renderer;
	}
}