#include "DeviceDX9.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "xaudio2.lib")


bool DeviceDX9::Initialize(const char* windowTitle, Utils::Vec2I windowSize)
{
	window = Utils::Window::Create(windowTitle, windowSize);

	// Initialize COM
	// COMの初期化
	CoInitializeEx(nullptr, 0);

	// Initialize Window
	// ウインドウの初期化
	window = Utils::Window::Create("DirectX9", Utils::Vec2I{1280, 720});

	// Initialize DirectX9
	// DirectX9の初期化
	D3DPRESENT_PARAMETERS d3dpParams{};
	d3dpParams.BackBufferWidth = window->GetWindowSize().X;
	d3dpParams.BackBufferHeight = window->GetWindowSize().Y;
	d3dpParams.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpParams.BackBufferCount = 1;
	d3dpParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpParams.Windowed = TRUE;
	d3dpParams.hDeviceWindow = (HWND)window->GetNativePtr(0);
	d3dpParams.EnableAutoDepthStencil = TRUE;
	d3dpParams.AutoDepthStencilFormat = D3DFMT_D16;

	d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

	d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, (HWND)window->GetNativePtr(0),
		D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpParams, d3d9Device.GetAddressOf());

	// Initialize COM
	// Initialize XAudio
	XAudio2Create(xa2Device.GetAddressOf());

	xa2Device->CreateMasteringVoice(&xa2MasterVoice);

	return true;
}

void DeviceDX9::Terminate()
{
	// Release XAudio2
	// XAudio2の解放
	if (xa2MasterVoice != nullptr)
	{
		xa2MasterVoice->DestroyVoice();
		xa2MasterVoice = nullptr;
	}
	xa2Device.Detach();

	// Release DirectX
	// DirectXの解放
	d3d9Device.Detach();
	d3d9.Detach();

	// Release COM
	// COMの解放
	CoUninitialize();
}

void DeviceDX9::ClearScreen()
{
	d3d9Device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	d3d9Device->BeginScene();
}

void DeviceDX9::PresentDevice()
{
	d3d9Device->EndScene();

	{
		HRESULT hr;
		hr = d3d9Device->Present(NULL, NULL, NULL, NULL);

		switch (hr)
		{
		case D3DERR_DEVICELOST:
			while (FAILED(hr = d3d9Device->TestCooperativeLevel()))
			{
				switch (hr)
				{
				case D3DERR_DEVICELOST:
					::SleepEx(1000, true);
					break;

				case D3DERR_DEVICENOTRESET:

					// Call it before device lost
					// デバイスロストの処理を行う前に実行する
					//onLostDevice();

					D3DPRESENT_PARAMETERS d3dpParams{};
					d3dpParams.BackBufferWidth = window->GetWindowSize().X;
					d3dpParams.BackBufferHeight = window->GetWindowSize().Y;
					d3dpParams.BackBufferFormat = D3DFMT_X8R8G8B8;
					d3dpParams.BackBufferCount = 1;
					d3dpParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
					d3dpParams.Windowed = TRUE;
					d3dpParams.hDeviceWindow = (HWND)window->GetNativePtr(0);
					d3dpParams.EnableAutoDepthStencil = TRUE;
					d3dpParams.AutoDepthStencilFormat = D3DFMT_D16;
					d3d9Device->Reset(&d3dpParams);

					// Call it after device lost
					// デバイスロストの処理の後に実行する
					//onResetDevice();

					break;
				}
			}
			break;
		}
	}
}
