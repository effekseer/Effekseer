
#include "efk.GraphicsDX9.h"

namespace efk
{
	RenderTextureDX9::RenderTextureDX9(Graphics* graphics)
		: graphics(graphics)
	{
	}

	RenderTextureDX9::~RenderTextureDX9()
	{
		ES_SAFE_RELEASE(renderTarget);
		ES_SAFE_RELEASE(renderTargetTexture);
	}

	bool RenderTextureDX9::Initialize(int32_t width, int32_t height, TextureFormat format, uint32_t multisample)
	{
		auto g = (GraphicsDX9*)graphics;
		auto r = (EffekseerRendererDX9::Renderer*)g->GetRenderer();
		
		D3DFORMAT d3dFormat;
		switch (format)
		{
		case TextureFormat::RGBA8U:
			d3dFormat = D3DFMT_A8R8G8B8;
			break;
		case TextureFormat::RGBA16F:
			d3dFormat = D3DFMT_A16B16G16R16F;
			break;
		case TextureFormat::R16F:
			d3dFormat = D3DFMT_R16F;
			break;
		default:
			assert(0);
			return false;
		}

		auto hr = r->GetDevice()->CreateTexture(
			width,
			height,
			1,
			D3DUSAGE_RENDERTARGET,
			d3dFormat,
			D3DPOOL_DEFAULT,
			&renderTargetTexture,
			NULL
		);

		if (FAILED(hr))
		{
			return false;
		}

		renderTargetTexture->GetSurfaceLevel(0, &renderTarget);
		this->width = width;
		this->height = height;

		return true;
	}

	DepthTextureDX9::DepthTextureDX9(Graphics* graphics)
		: graphics(graphics)
	{
	}

	DepthTextureDX9::~DepthTextureDX9()
	{
		ES_SAFE_RELEASE(depthTexture);
	}

	bool DepthTextureDX9::Initialize(int32_t width, int32_t height, uint32_t multisample)
	{
		auto g = (GraphicsDX9*)graphics;
		auto r = (EffekseerRendererDX9::Renderer*)g->GetRenderer();
		auto hr = r->GetDevice()->CreateDepthStencilSurface(width, height, D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, TRUE, &depthTexture, NULL);

		if (FAILED(hr))
		{
			return false;
		}

		this->width = width;
		this->height = height;

		return true;
	}

	GraphicsDX9::GraphicsDX9()
	{

	}

	GraphicsDX9::~GraphicsDX9()
	{
		if (renderer != nullptr)
		{
			renderer->Destroy();
			renderer = nullptr;
		}

		assert(renderDefaultTarget == nullptr);
		assert(recordingTarget == nullptr);
		
		ES_SAFE_RELEASE(backTarget);
		ES_SAFE_RELEASE(backTargetTexture);

		ES_SAFE_RELEASE(d3d_device);
		ES_SAFE_RELEASE(d3d);
	}

	bool GraphicsDX9::Initialize(void* windowHandle, int32_t windowWidth, int32_t windowHeight, bool isSRGBMode, int32_t spriteCount)
	{
		HRESULT hr;

		D3DPRESENT_PARAMETERS d3dp;
		ZeroMemory(&d3dp, sizeof(d3dp));
		d3dp.BackBufferWidth = windowWidth;
		d3dp.BackBufferHeight = windowHeight;
		d3dp.BackBufferFormat = D3DFMT_X8R8G8B8;
		d3dp.BackBufferCount = 1;
		d3dp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dp.Windowed = TRUE;
		d3dp.hDeviceWindow = (HWND)windowHandle;
		d3dp.EnableAutoDepthStencil = TRUE;
		d3dp.AutoDepthStencilFormat = D3DFMT_D16;

		d3d = Direct3DCreate9(D3D_SDK_VERSION);
		if (d3d == NULL) return false;

		D3DDEVTYPE	deviceTypes[4];
		DWORD	flags[4];

		deviceTypes[0] = D3DDEVTYPE_HAL;
		flags[0] = D3DCREATE_HARDWARE_VERTEXPROCESSING;
		deviceTypes[1] = D3DDEVTYPE_HAL;
		flags[1] = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		deviceTypes[2] = D3DDEVTYPE_REF;
		flags[2] = D3DCREATE_HARDWARE_VERTEXPROCESSING;
		deviceTypes[3] = D3DDEVTYPE_REF;
		flags[3] = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

		for (int ind = 0; ind < 4; ind++)
		{
			hr = d3d->CreateDevice(
				D3DADAPTER_DEFAULT,
				deviceTypes[ind],
				(HWND)windowHandle,
				flags[ind],
				&d3dp,
				&d3d_device);
			if (SUCCEEDED(hr)) break;
		}

		if (FAILED(hr))
		{
			ES_SAFE_RELEASE(d3d_device);
			ES_SAFE_RELEASE(d3d);
			return false;
		}

		this->isSRGBMode = isSRGBMode;
		this->windowHandle = windowHandle;
		this->windowWidth = windowWidth;
		this->windowHeight = windowHeight;

		renderer = ::EffekseerRendererDX9::Renderer::Create(d3d_device, spriteCount);

		return true;
	}

	void GraphicsDX9::CopyToBackground()
	{
		bool ret = false;

		HRESULT hr;

		IDirect3DSurface9* tempRender = nullptr;
		IDirect3DSurface9* tempDepth = nullptr;

		hr = d3d_device->GetRenderTarget(0, &tempRender);
		if (FAILED(hr))
		{
			goto Exit;
		}

		hr = d3d_device->GetDepthStencilSurface(&tempDepth);
		if (FAILED(hr))
		{
			goto Exit;
		}

		D3DSURFACE_DESC temp_desc;
		D3DSURFACE_DESC background_desc;

		tempRender->GetDesc(&temp_desc);

		if (backTarget != nullptr)
		{
			backTarget->GetDesc(&background_desc);
		}

		if (backTarget == nullptr || temp_desc.Width != background_desc.Width || temp_desc.Height != background_desc.Height)
		{
			ES_SAFE_RELEASE(backTarget);
			ES_SAFE_RELEASE(backTargetTexture);

			hr = d3d_device->CreateTexture(
				temp_desc.Width,
				temp_desc.Height,
				1,
				D3DUSAGE_RENDERTARGET,
				D3DFMT_A8R8G8B8,
				D3DPOOL_DEFAULT,
				&backTargetTexture,
				NULL
			);

			if (FAILED(hr))
			{
				goto Exit;
			}

			backTargetTexture->GetSurfaceLevel(0, &backTarget);
		}

		d3d_device->SetRenderTarget(0, backTarget);
		d3d_device->SetDepthStencilSurface(nullptr);

		hr = d3d_device->StretchRect(
			tempRender,
			nullptr,
			backTarget,
			nullptr,
			D3DTEXF_POINT);

		d3d_device->SetRenderTarget(0, tempRender);
		d3d_device->SetDepthStencilSurface(tempDepth);

		ret = true;

	Exit:;
		ES_SAFE_RELEASE(tempRender);
		ES_SAFE_RELEASE(tempDepth);
	}

	void GraphicsDX9::Resize(int32_t width, int32_t height)
	{
		this->windowWidth = width;
		this->windowHeight = height;
		ResetDevice();
	}

	bool GraphicsDX9::Present()
	{
		HRESULT hr;

		// gamma
		if (isSRGBMode)
		{
			IDirect3DSwapChain9* swapChain = nullptr;
			d3d_device->GetSwapChain(0, &swapChain);

			hr = swapChain->Present(nullptr, nullptr, nullptr, nullptr, D3DPRESENT_LINEAR_CONTENT);

			ES_SAFE_RELEASE(swapChain);
		}
		else
		{
			hr = d3d_device->Present(NULL, NULL, NULL, NULL);
		}

		switch (hr)
		{
			// cause an unknown error
		case D3DERR_DRIVERINTERNALERROR:
			return false;

			// device lost
		case D3DERR_DEVICELOST:
			while (FAILED(hr = d3d_device->TestCooperativeLevel()))
			{
				switch (hr)
				{
					// device lost
				case D3DERR_DEVICELOST:
					::SleepEx(1000, true);
					break;
					// device lost : reset
				case D3DERR_DEVICENOTRESET:
					ResetDevice();
					break;
				}
			}
			break;
		}

		if (Presented != nullptr)
		{
			Presented();
		}

		return true;
	}

	void GraphicsDX9::BeginScene()
	{
		d3d_device->BeginScene();

		if (isSRGBMode)
		{
			d3d_device->SetSamplerState(0, D3DSAMP_SRGBTEXTURE, TRUE);
		}
	}

	void GraphicsDX9::EndScene()
	{
		if (isSRGBMode)
		{
			d3d_device->SetSamplerState(0, D3DSAMP_SRGBTEXTURE, FALSE);
		}

		d3d_device->EndScene();
	}

	void GraphicsDX9::SetRenderTarget(RenderTexture* renderTexture, DepthTexture* depthTexture)
	{
		currentRenderTexture = renderTexture;
		currentDepthTexture = depthTexture;

		auto rt = (RenderTextureDX9*)renderTexture;
		auto dt = (DepthTextureDX9*)depthTexture;

		if (renderTexture == nullptr)
		{
			d3d_device->SetRenderTarget(0, renderDefaultTarget);
			d3d_device->SetDepthStencilSurface(renderDefaultDepth);
			ES_SAFE_RELEASE(renderDefaultTarget);
			ES_SAFE_RELEASE(renderDefaultDepth);
		}
		else
		{
			d3d_device->GetRenderTarget(0, &renderDefaultTarget);
			d3d_device->GetDepthStencilSurface(&renderDefaultDepth);

			if (rt != nullptr)
			{
				d3d_device->SetRenderTarget(0, rt->GetSurface());
			}
			else
			{
				d3d_device->SetRenderTarget(0, nullptr);
			}
			
			if (dt != nullptr)
			{
				d3d_device->SetDepthStencilSurface(dt->GetSurface());
			}
			else
			{
				d3d_device->SetDepthStencilSurface(nullptr);
			}
		}
	}

	void GraphicsDX9::BeginRecord(int32_t width, int32_t height)
	{
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
	}

	void GraphicsDX9::EndRecord(std::vector<Effekseer::Color>& pixels)
	{
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
	}

	void GraphicsDX9::Clear(Effekseer::Color color)
	{
		d3d_device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(color.R, color.G, color.B, color.A), 1.0f, 0);
	}

	void GraphicsDX9::ResetDevice()
	{
		ES_SAFE_RELEASE(backTarget);
		ES_SAFE_RELEASE(backTargetTexture);

		renderer->OnLostDevice();

		if (LostedDevice != nullptr)
		{
			LostedDevice();
		}

		HRESULT hr;

		D3DPRESENT_PARAMETERS d3dp;
		ZeroMemory(&d3dp, sizeof(d3dp));
		d3dp.BackBufferWidth = windowWidth;
		d3dp.BackBufferHeight = windowHeight;
		d3dp.BackBufferFormat = D3DFMT_X8R8G8B8;
		d3dp.BackBufferCount = 1;
		d3dp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dp.Windowed = TRUE;
		d3dp.hDeviceWindow = (HWND)windowHandle;
		d3dp.EnableAutoDepthStencil = TRUE;
		d3dp.AutoDepthStencilFormat = D3DFMT_D16;

		hr = d3d_device->Reset(&d3dp);

		if (FAILED(hr))
		{
			assert(0);
			return;
		}

		if (ResettedDevice != nullptr)
		{
			ResettedDevice();
		}

		renderer->OnResetDevice();
	}

	void* GraphicsDX9::GetBack()
	{
		return backTargetTexture;
	}

	EffekseerRenderer::Renderer* GraphicsDX9::GetRenderer()
	{
		return renderer;
	}
}