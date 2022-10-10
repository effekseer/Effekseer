#include "DeviceDX11.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "xaudio2.lib")

bool DeviceDX11::Initialize(const char* windowTitle, Utils::Vec2I windowSize)
{
	window = Utils::Window::Create(windowTitle, windowSize);

	// Initialize COM
	// COMの初期化
	CoInitializeEx(nullptr, 0);

	// Initialize DirectX11
	// DirectX11の初期化
	UINT debugFlag = 0;
	// debugFlag = D3D11_CREATE_DEVICE_DEBUG;

	if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, debugFlag, nullptr, 0, D3D11_SDK_VERSION, dx11Device.GetAddressOf(), nullptr, dx11Context.GetAddressOf())))
	{
		Terminate();
		return false;
	}

	if (FAILED(dx11Device->QueryInterface(__uuidof(IDXGIDevice1), (void**)&dxgiDevice)))
	{
		Terminate();
		return false;
	}

	if (FAILED(dxgiDevice->GetAdapter(&adapter)))
	{
		Terminate();
		return false;
	}

	adapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);
	if (dxgiFactory == nullptr)
	{
		Terminate();
		return false;
	}

	DXGI_SWAP_CHAIN_DESC hDXGISwapChainDesc{};
	hDXGISwapChainDesc.BufferDesc.Width = window->GetWindowSize().X;
	hDXGISwapChainDesc.BufferDesc.Height = window->GetWindowSize().Y;
	hDXGISwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	hDXGISwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	hDXGISwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	hDXGISwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	hDXGISwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	hDXGISwapChainDesc.SampleDesc.Count = 1;
	hDXGISwapChainDesc.SampleDesc.Quality = 0;
	hDXGISwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	hDXGISwapChainDesc.BufferCount = 1;
	hDXGISwapChainDesc.OutputWindow = (HWND)window->GetNativePtr(0);
	hDXGISwapChainDesc.Windowed = TRUE;
	hDXGISwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	hDXGISwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	if (FAILED(dxgiFactory->CreateSwapChain(dx11Device.Get(), &hDXGISwapChainDesc, swapChain.GetAddressOf())))
	{
		Terminate();
		return false;
	}

	if (FAILED(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer)))
	{
		Terminate();
		return false;
	}

	if (FAILED(dx11Device->CreateRenderTargetView(backBuffer.Get(), nullptr, renderTargetView.GetAddressOf())))
	{
		Terminate();
		return false;
	}

	D3D11_TEXTURE2D_DESC hTexture2dDesc{};
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
	if (FAILED(dx11Device->CreateTexture2D(&hTexture2dDesc, nullptr, &depthBuffer)))
	{
		Terminate();
		return false;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC hDepthStencilViewDesc{};
	hDepthStencilViewDesc.Format = hTexture2dDesc.Format;
	hDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	hDepthStencilViewDesc.Flags = 0;
	if (FAILED(dx11Device->CreateDepthStencilView(depthBuffer.Get(), &hDepthStencilViewDesc, depthStencilView.GetAddressOf())))
	{
		Terminate();
		return false;
	}

	ID3D11RenderTargetView* renderTargetViews[] = {renderTargetView.Get()};
	dx11Context->OMSetRenderTargets(1, renderTargetViews, depthStencilView.Get());

	D3D11_VIEWPORT viewport{};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (float)window->GetWindowSize().X;
	viewport.Height = (float)window->GetWindowSize().Y;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	dx11Context->RSSetViewports(1, &viewport);

	// Initialize COM
	// Initialize XAudio
	XAudio2Create(&xa2Device);

	xa2Device->CreateMasteringVoice(&xa2MasterVoice);

	return true;
}

void DeviceDX11::Terminate()
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
	renderTargetView.Detach();
	backBuffer.Detach();
	depthStencilView.Detach();
	depthBuffer.Detach();
	swapChain.Detach();
	dxgiFactory.Detach();
	adapter.Detach();
	dxgiDevice.Detach();
	dx11Context.Detach();
	dx11Device.Detach();

	// Release COM
	// COMの解放
	CoUninitialize();
}

void DeviceDX11::ClearScreen()
{
	float ClearColor[] = {0.0f, 0.0f, 0.0f, 1.0f};
	dx11Context->ClearRenderTargetView(renderTargetView.Get(), ClearColor);
	dx11Context->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void DeviceDX11::PresentDevice()
{
	swapChain->Present(1, 0);
}

bool DeviceDX11::NewFrame()
{
	bool result = window->OnNewFrame();

	bool windowActive = GetActiveWindow() == window->GetNativePtr(0);
	for (int key = 0; key < 256; key++)
	{
		Utils::Input::UpdateKeyState(key, windowActive && (GetAsyncKeyState(key) & 0x01) != 0);
	}

	return result;
}

void DeviceDX11::SetupEffekseerModules(::Effekseer::ManagerRef efkManager)
{
	// Create a  graphics device
	// 描画デバイスの作成
	auto graphicsDevice = ::EffekseerRendererDX11::CreateGraphicsDevice(GetID3D11Device(), GetID3D11DeviceContext());

	// Create a renderer of effects
	// エフェクトのレンダラーの作成
	efkRenderer = ::EffekseerRendererDX11::Renderer::Create(graphicsDevice, 8000);

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
