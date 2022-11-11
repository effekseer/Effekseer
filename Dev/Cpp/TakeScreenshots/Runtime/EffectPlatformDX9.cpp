#include "EffectPlatformDX9.h"
#include "../../3rdParty/stb/stb_image_write.h"
#include <assert.h>

DistortingCallbackDX9::DistortingCallbackDX9(::EffekseerRendererDX9::RendererRef renderer,
											 LPDIRECT3DDEVICE9 device,
											 int texWidth,
											 int texHeight)
	: device(device)
	, texWidth_(texWidth)
	, texHeight_(texHeight)
{
	device->CreateTexture(texWidth, texHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &texture, NULL);
}

DistortingCallbackDX9::~DistortingCallbackDX9()
{
	ES_SAFE_RELEASE(texture);
}

bool DistortingCallbackDX9::OnDistorting(EffekseerRenderer::Renderer* renderer)
{
	IDirect3DSurface9* targetSurface = nullptr;
	IDirect3DSurface9* texSurface = nullptr;
	HRESULT hr = S_OK;

	hr = texture->GetSurfaceLevel(0, &texSurface);
	assert(SUCCEEDED(hr));

	hr = device->GetRenderTarget(0, &targetSurface);
	assert(SUCCEEDED(hr));

	hr = device->StretchRect(targetSurface, NULL, texSurface, NULL, D3DTEXF_NONE);
	assert(SUCCEEDED(hr));

	ES_SAFE_RELEASE(texSurface);
	ES_SAFE_RELEASE(targetSurface);

	reinterpret_cast<EffekseerRendererDX9::Renderer*>(renderer)->SetBackground(texture);

	return true;
}

void DistortingCallbackDX9::Lost()
{
	ES_SAFE_RELEASE(texture);
}

void DistortingCallbackDX9::ChangeDevice(LPDIRECT3DDEVICE9 device)
{
	Effekseer::SafeAddRef(device);
	Effekseer::SafeRelease(this->device);
	this->device = device;
}

void DistortingCallbackDX9::Reset()
{
	device->CreateTexture(texWidth_, texHeight_, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &texture, NULL);
}

void EffectPlatformDX9::CreateCheckedSurface()
{
	device_->CreateOffscreenPlainSurface(initParam_.WindowSize[0], initParam_.WindowSize[1], D3DFMT_X8R8G8B8, D3DPOOL_SYSTEMMEM, &checkedSurface_, nullptr);
	D3DLOCKED_RECT lockedRect;
	checkedSurface_->LockRect(&lockedRect, nullptr, 0);
	memcpy(lockedRect.pBits, checkeredPattern_.data(), checkeredPattern_.size() * sizeof(uint32_t));
	checkedSurface_->UnlockRect();
}

EffekseerRenderer::RendererRef EffectPlatformDX9::CreateRenderer()
{
	auto ret = EffekseerRendererDX9::Renderer::Create(device_, initParam_.SpriteCount);
	distorting_ = new DistortingCallbackDX9((EffekseerRendererDX9::RendererRef)ret, device_, initParam_.WindowSize[0], initParam_.WindowSize[1]);
	ret->SetDistortingCallback(distorting_);
	return ret;
}

EffectPlatformDX9::~EffectPlatformDX9()
{
	ES_SAFE_RELEASE(checkedSurface_);
	ES_SAFE_RELEASE(d3d_);
	ES_SAFE_RELEASE(device_);
}

void EffectPlatformDX9::InitializeDevice(const EffectPlatformInitializingParameter& param)
{
	HRESULT hr;

	D3DPRESENT_PARAMETERS d3dp;
	ZeroMemory(&d3dp, sizeof(d3dp));
	d3dp.BackBufferWidth = initParam_.WindowSize[0];
	d3dp.BackBufferHeight = initParam_.WindowSize[1];
	d3dp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dp.BackBufferCount = 1;
	d3dp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dp.Windowed = TRUE;
	d3dp.hDeviceWindow = (HWND)GetNativePtr(0);
	d3dp.EnableAutoDepthStencil = TRUE;
	d3dp.AutoDepthStencilFormat = D3DFMT_D16;

	vsync_ = param.VSync;
	if (!vsync_)
	{
		d3dp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	}

	d3d_ = Direct3DCreate9(D3D_SDK_VERSION);

	if (d3d_ == nullptr)
	{
		throw "Failed : Direct3DCreate9";
	}

	hr =
		d3d_->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, (HWND)GetNativePtr(0), D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dp, &device_);

	if (FAILED(hr))
	{
		throw "Failed : CreateDevice";
	}

	CreateCheckedSurface();
}

void EffectPlatformDX9::BeginRendering()
{
	device_->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	LPDIRECT3DSURFACE9 targetSurface = nullptr;
	device_->GetRenderTarget(0, &targetSurface);
	device_->UpdateSurface(checkedSurface_, NULL, targetSurface, NULL);
	ES_SAFE_RELEASE(targetSurface);

	device_->BeginScene();
}

void EffectPlatformDX9::EndRendering()
{
	device_->EndScene();
}

void EffectPlatformDX9::Present()
{
	auto hr = device_->Present(nullptr, nullptr, nullptr, nullptr);

	switch (hr)
	{
		// cause an unknown error
	case D3DERR_DRIVERINTERNALERROR:
		throw "Failed : D3DERR_DRIVERINTERNALERROR";

		// device lost
	case D3DERR_DEVICELOST:
		while (FAILED(hr = device_->TestCooperativeLevel()))
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
}

bool EffectPlatformDX9::TakeScreenshot(const char* path)
{

	IDirect3DSurface9* surface = nullptr;
	device_->CreateOffscreenPlainSurface(initParam_.WindowSize[0], initParam_.WindowSize[1], D3DFMT_X8R8G8B8, D3DPOOL_SYSTEMMEM, &surface, nullptr);

	LPDIRECT3DSURFACE9 backBuf;
	device_->GetRenderTarget(0, &backBuf);

	device_->GetRenderTargetData(backBuf, surface);
	backBuf->Release();

	D3DLOCKED_RECT locked;
	RECT rect;
	rect.left = 0;
	rect.bottom = initParam_.WindowSize[1];
	rect.top = 0;
	rect.right = initParam_.WindowSize[0];
	surface->LockRect(&locked, &rect, D3DLOCK_NO_DIRTY_UPDATE | D3DLOCK_NOSYSLOCK | D3DLOCK_READONLY);

	std::vector<uint8_t> data;

	data.resize(initParam_.WindowSize[0] * initParam_.WindowSize[1] * 4);

	for (int32_t h = 0; h < initParam_.WindowSize[1]; h++)
	{
		auto dst_ = &(data[h * initParam_.WindowSize[0] * 4]);
		auto src_ = &(((uint8_t*)locked.pBits)[h * locked.Pitch]);
		memcpy(dst_, src_, initParam_.WindowSize[0] * 4);
	}

	// HACK for Geforce
	for (int32_t i = 0; i < initParam_.WindowSize[0] * initParam_.WindowSize[1]; i++)
	{
		data[i * 4 + 3] = 255;
		std::swap(data[i * 4 + 0], data[i * 4 + 2]);
	}

	surface->UnlockRect();
	surface->Release();

	stbi_write_png(path, initParam_.WindowSize[0], initParam_.WindowSize[1], 4, data.data(), initParam_.WindowSize[0] * 4);

	return true;
}

bool EffectPlatformDX9::SetFullscreen(bool isFullscreen)
{
	fullscreen_ = !fullscreen_;
	ResetDevice();
	return true;
}

void EffectPlatformDX9::ResetDevice()
{
	ES_SAFE_RELEASE(checkedSurface_);

	distorting_->Lost();

	auto renderer = static_cast<EffekseerRendererDX9::Renderer*>(GetRenderer().Get());

	for (size_t i = 0; i < effects_.size(); i++)
	{
		effects_[i]->UnloadResources();
	}

	renderer->OnLostDevice();

	HRESULT hr;

	D3DPRESENT_PARAMETERS d3dp;
	ZeroMemory(&d3dp, sizeof(d3dp));
	d3dp.BackBufferWidth = initParam_.WindowSize[0];
	d3dp.BackBufferHeight = initParam_.WindowSize[1];
	d3dp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dp.BackBufferCount = 1;
	d3dp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dp.Windowed = fullscreen_ ? FALSE : TRUE;
	d3dp.hDeviceWindow = (HWND)GetNativePtr(0);
	d3dp.EnableAutoDepthStencil = TRUE;
	d3dp.AutoDepthStencilFormat = D3DFMT_D16;
	if (!vsync_)
	{
		d3dp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	}

	bool newDevice = true;

	if (newDevice)
	{
		distorting_->ChangeDevice(nullptr);
		renderer->ChangeDevice(nullptr);
		device_->Release();

		hr =
			d3d_->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, (HWND)GetNativePtr(0), D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dp, &device_);

		if (FAILED(hr))
		{
			throw "Failed : CreateDevice";
		}

		renderer->ChangeDevice(device_);
		distorting_->ChangeDevice(device_);
	}
	else
	{
		hr = device_->Reset(&d3dp);

		if (FAILED(hr))
		{
			throw "Failed : ResetDevice";
			return;
		}
	}

	distorting_->Reset();

	renderer->OnResetDevice();

	for (size_t i = 0; i < effects_.size(); i++)
	{
		effects_[i]->ReloadResources(buffers_[i].data(), static_cast<int32_t>(buffers_[i].size()));
	}

	CreateCheckedSurface();
}
