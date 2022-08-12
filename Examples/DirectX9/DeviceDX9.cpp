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
	window = Utils::Window::Create(windowTitle, windowSize);

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

	d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, (HWND)window->GetNativePtr(0), D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpParams, d3d9Device.GetAddressOf());

	// Initialize COM
	// Initialize XAudio
	XAudio2Create(xa2Device.GetAddressOf());

	xa2Device->CreateMasteringVoice(&xa2MasterVoice);

	return true;
}

void DeviceDX9::Terminate()
{
	efkRenderer.Reset();
	efkSound.Reset();

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
					if (onLostDevice)
					{
						onLostDevice();
					}

					efkRenderer->OnLostDevice();

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
					efkRenderer->OnResetDevice();

					if (onResetDevice)
					{
						onResetDevice();
					}

					break;
				}
			}
			break;
		}
	}
}

bool DeviceDX9::NewFrame()
{
	bool result = window->OnNewFrame();

	bool windowActive = GetActiveWindow() == window->GetNativePtr(0);
	for (int key = 0; key < 256; key++)
	{
		Utils::Input::UpdateKeyState(key, windowActive && (GetAsyncKeyState(key) & 0x01) != 0);
	}

	return result;
}

void DeviceDX9::SetupEffekseerModules(::Effekseer::ManagerRef efkManager)
{
	// Create a  graphics device
	// 描画デバイスの作成
	auto graphicsDevice = ::EffekseerRendererDX9::CreateGraphicsDevice(GetIDirect3DDevice9());

	// Create a renderer of effects
	// エフェクトのレンダラーの作成
	efkRenderer = ::EffekseerRendererDX9::Renderer::Create(graphicsDevice, 8000);

	// Sprcify rendering modules
	// 描画モジュールの設定
	efkManager->SetSpriteRenderer(efkRenderer->CreateSpriteRenderer());
	efkManager->SetRibbonRenderer(efkRenderer->CreateRibbonRenderer());
	efkManager->SetRingRenderer(efkRenderer->CreateRingRenderer());
	efkManager->SetTrackRenderer(efkRenderer->CreateTrackRenderer());
	efkManager->SetModelRenderer(efkRenderer->CreateModelRenderer());

	// Specify a texture, model, curve and material loader
	// It can be extended by yourself. It is loaded from a file on now.
	// テクスチャ、モデル、カーブ、マテリアルローダーの設定する。
	// ユーザーが独自で拡張できる。現在はファイルから読み込んでいる。
	efkManager->SetTextureLoader(efkRenderer->CreateTextureLoader());
	efkManager->SetModelLoader(efkRenderer->CreateModelLoader());
	efkManager->SetMaterialLoader(efkRenderer->CreateMaterialLoader());
	efkManager->SetCurveLoader(Effekseer::MakeRefPtr<Effekseer::CurveLoader>());

	// Specify sound modules
	// サウンドモジュールの設定
	efkSound = ::EffekseerSound::Sound::Create(GetIXAudio2(), 16, 16);

	// Specify a metho to play sound from an instance of efkSound
	// 音再生用インスタンスから再生機能を指定
	efkManager->SetSoundPlayer(efkSound->CreateSoundPlayer());

	// Specify a sound data loader
	// It can be extended by yourself. It is loaded from a file on now.
	// サウンドデータの読込機能を設定する。
	// ユーザーが独自で拡張できる。現在はファイルから読み込んでいる。
	efkManager->SetSoundLoader(efkSound->CreateSoundLoader());
}
