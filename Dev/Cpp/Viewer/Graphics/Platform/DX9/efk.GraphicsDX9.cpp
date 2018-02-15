
#include "efk.GraphicsDX9.h"

namespace efk
{
	GraphicsDX9::GraphicsDX9()
	{

	}

	GraphicsDX9::~GraphicsDX9()
	{
		ES_SAFE_RELEASE(d3d_device);
		ES_SAFE_RELEASE(d3d);
	}

	bool GraphicsDX9::Initialize(void* windowHandle, int32_t windowWidth, int32_t windowHeight, bool isSRGBMode)
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

		return true;
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

		return true;
	}

	void GraphicsDX9::ResetDevice()
	{
		/*
		GenerateRenderTargets(0, 0);
		ES_SAFE_RELEASE(m_backGroundTexture);

		if (LostedDevice != nullptr)
		{
			LostedDevice();
		}

		m_renderer->OnLostDevice();

		HRESULT hr;

		D3DPRESENT_PARAMETERS d3dp;
		ZeroMemory(&d3dp, sizeof(d3dp));
		d3dp.BackBufferWidth = m_width;
		d3dp.BackBufferHeight = m_height;
		d3dp.BackBufferFormat = D3DFMT_X8R8G8B8;
		d3dp.BackBufferCount = 1;
		d3dp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dp.Windowed = TRUE;
		d3dp.hDeviceWindow = m_handle;
		d3dp.EnableAutoDepthStencil = TRUE;
		d3dp.AutoDepthStencilFormat = D3DFMT_D16;


		hr = d3d_device->Reset(&d3dp);

		if (FAILED(hr))
		{
			assert(0);
			return;
		}

		m_renderer->OnResetDevice();

		if (ResettedDevice != nullptr)
		{
			ResettedDevice();
		}

		LoadBackgroundImageInternal(m_backGroundTextureBuffer.data(), m_backGroundTextureBuffer.size());
		GenerateRenderTargets(m_width, m_height);
		*/
	}

}