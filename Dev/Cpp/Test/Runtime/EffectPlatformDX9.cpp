#include "EffectPlatformDX9.h"

EffekseerRenderer::Renderer* EffectPlatformDX9::CreateRenderer() { return nullptr; }

EffectPlatformDX9::~EffectPlatformDX9()
{
	ES_SAFE_RELEASE(d3d_);
	ES_SAFE_RELEASE(device_);
}

void EffectPlatformDX9::InitializeDevice(const EffectPlatformInitializingParameter& param)
{
	HRESULT hr;

	D3DPRESENT_PARAMETERS d3dp;
	ZeroMemory(&d3dp, sizeof(d3dp));
	d3dp.BackBufferWidth = 1280;
	d3dp.BackBufferHeight = 720;
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
}

void EffectPlatformDX9::BeginRendering()
{
	device_->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	device_->BeginScene();
}

void EffectPlatformDX9::EndRendering() { device_->EndScene(); }

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

bool EffectPlatformDX9::TakeScreenshot(const char* path) { return false; }

bool EffectPlatformDX9::SetFullscreen(bool isFullscreen)
{
	fullscreen_ = !fullscreen_;
	ResetDevice();
	return true;
}

void EffectPlatformDX9::ResetDevice()
{

	auto renderer = static_cast<EffekseerRendererDX9::Renderer*>(GetRenderer());

	for (size_t i = 0; i < effects_.size(); i++)
	{
		effects_[i]->UnloadResources();
	}

	renderer->OnLostDevice();

	HRESULT hr;

	D3DPRESENT_PARAMETERS d3dp;
	ZeroMemory(&d3dp, sizeof(d3dp));
	d3dp.BackBufferWidth = 1280;
	d3dp.BackBufferHeight = 720;
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

	hr = device_->Reset(&d3dp);

	if (FAILED(hr))
	{
		throw "Failed : ResetDevice";
		return;
	}

	renderer->OnResetDevice();

	for (size_t i = 0; i < effects_.size(); i++)
	{
		effects_[i]->ReloadResources(buffers_[i].data(), buffers_[i].size());
	}
}
