#include "DeviceDX12.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "xaudio2.lib")

bool DeviceDX12::Initialize(const char* windowTitle, Utils::Vec2I windowSize)
{
	// Initialize COM
	// COMの初期化
	CoInitializeEx(nullptr, 0);

	// A code to initialize DirectX12 is too long, so I use LLGI
	// DirectX12初期化のためのコードは長すぎるのでLLGIを使用する。
	LLGI::PlatformParameter platformParam{};
	platformParam.Device = LLGI::DeviceType::DirectX12;
	platformParam.WaitVSync = true;

	window = std::shared_ptr<LLGI::Window>(LLGI::CreateWindow(windowTitle, {windowSize.X, windowSize.Y}));
	if (window == nullptr)
	{
		Terminate();
		return false;
	}

	platform = LLGI::CreateSharedPtr(LLGI::CreatePlatform(platformParam, window.get()));
	if (platform == nullptr)
	{
		Terminate();
		return false;
	}

	graphics = LLGI::CreateSharedPtr(platform->CreateGraphics());
	if (graphics == nullptr)
	{
		Terminate();
		return false;
	}

	memoryPool = LLGI::CreateSharedPtr(graphics->CreateSingleFrameMemoryPool(1024 * 1024, 128));
	commandListPool = std::make_shared<LLGI::CommandListPool>(graphics.get(), memoryPool.get(), 3);

	// Initialize COM
	// Initialize XAudio
	XAudio2Create(&xa2Device);

	xa2Device->CreateMasteringVoice(&xa2MasterVoice);

	return true;
}

void DeviceDX12::Terminate()
{
	// Release XAudio2
	// XAudio2の解放
	if (xa2MasterVoice != nullptr)
	{
		xa2MasterVoice->DestroyVoice();
		xa2MasterVoice = nullptr;
	}
	xa2Device.Detach();

	efkCommandList.Reset();
	efkMemoryPool.Reset();
	efkRenderer.Reset();

	commandListPool.reset();
	memoryPool.reset();
	graphics.reset();
	platform.reset();
	window.reset();

	// Release COM
	// COMの解放
	CoUninitialize();
}

void DeviceDX12::ClearScreen()
{
}

bool DeviceDX12::NewFrame()
{
	if (!platform->NewFrame())
		return false;

	memoryPool->NewFrame();

	commandList = commandListPool->Get();

	LLGI::Color8 color;
	color.R = 0;
	color.G = 0;
	color.B = 0;
	color.A = 255;

	commandList->Begin();
	commandList->BeginRenderPass(platform->GetCurrentScreen(color, true, false)); // TODO: isDepthClear is false, because it fails with dx12.

	// Call on starting of a frame
	// フレームの開始時に呼ぶ
	efkMemoryPool->NewFrame();

	// Begin a command list
	// コマンドリストを開始する。
	EffekseerRendererDX12::BeginCommandList(efkCommandList, GetCommandList());
	efkRenderer->SetCommandList(efkCommandList);

	return true;
}

void DeviceDX12::PresentDevice()
{
	// Finish a command list
	// コマンドリストを終了する。
	efkRenderer->SetCommandList(nullptr);
	EffekseerRendererDX12::EndCommandList(efkCommandList);

	commandList->EndRenderPass();
	commandList->End();

	graphics->Execute(commandList);

	platform->Present();
}

void DeviceDX12::SetupEffekseerModules(::Effekseer::ManagerRef efkManager)
{
	// Create a  graphics device
	// 描画デバイスの作成
	auto graphicsDevice = ::EffekseerRendererDX12::CreateGraphicsDevice(GetID3D12Device(), GetCommandQueue(), 3);

	// Create a renderer of effects
	// エフェクトのレンダラーの作成
	auto format = DXGI_FORMAT_R8G8B8A8_UNORM;
	efkRenderer = ::EffekseerRendererDX12::Create(graphicsDevice, &format, 1, DXGI_FORMAT_UNKNOWN, false, 8000);

	// Create a memory pool
	// メモリプールの作成
	efkMemoryPool = EffekseerRenderer::CreateSingleFrameMemoryPool(efkRenderer->GetGraphicsDevice());

	// Create a command list
	// コマンドリストの作成
	efkCommandList = EffekseerRenderer::CreateCommandList(efkRenderer->GetGraphicsDevice(), efkMemoryPool);

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
}
