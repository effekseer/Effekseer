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
	frames.resize(platform->GetMaxFrameCount());
	for (auto& frame : frames)
	{
		frame.Native = LLGI::CreateSharedPtr(graphics->CreateCommandList(memoryPool.get()));
	}

	// Initialize COM
	// Initialize XAudio
	XAudio2Create(&xa2Device);

	xa2Device->CreateMasteringVoice(&xa2MasterVoice);

	return true;
}

void DeviceDX12::Terminate()
{
	if (graphics)
	{
		graphics->WaitFinish();
	}

	// Release XAudio2
	// XAudio2の解放
	if (xa2MasterVoice != nullptr)
	{
		xa2MasterVoice->DestroyVoice();
		xa2MasterVoice = nullptr;
	}
	xa2Device.Detach();

	efkCommandList.Reset();
	frames.clear();
	commandList = nullptr;
	nextFrame = 0;
	efkMemoryPool.Reset();
	efkRenderer.Reset();

	memoryPool.reset();
	graphics.reset();
	platform.reset();
	window.reset();

	// Release COM
	// COMの解放
	CoUninitialize();
}

bool DeviceDX12::NewFrame()
{
	if (!platform->NewFrame())
		return false;

	// Both memory pools advance in the same order as these frame slots.
	// Wait only for the slot being reused, not for other in-flight frames.
	auto& frame = frames[nextFrame];
	if (frame.Submitted)
	{
		frame.Native->WaitUntilCompleted();
	}
	commandList = frame.Native.get();
	efkCommandList = frame.Effekseer;
	memoryPool->NewFrame();

	// Call on starting of a frame
	// フレームの開始時に呼ぶ
	efkMemoryPool->NewFrame();

	commandList->Begin();

	// Begin a command list
	// コマンドリストを開始する。
	EffekseerRendererDX12::BeginCommandList(efkCommandList, GetCommandList());
	efkRenderer->SetCommandList(efkCommandList);

	bool windowActive = GetActiveWindow() == window->GetNativePtr(0);
	for (int key = 0; key < 256; key++)
	{
		Utils::Input::UpdateKeyState(key, windowActive && (GetAsyncKeyState(key) & 0x01) != 0);
	}

	return true;
}

void DeviceDX12::BeginComputePass()
{
	commandList->BeginComputePass();
}

void DeviceDX12::EndComputePass()
{
	commandList->EndComputePass();
}

void DeviceDX12::BeginRenderPass()
{
	// TODO: isDepthClear is false, because it fails with dx12.
	LLGI::Color8 color{0, 0, 0, 255};
	commandList->BeginRenderPass(platform->GetCurrentScreen(color, true, false));
}

void DeviceDX12::EndRenderPass()
{
	commandList->EndRenderPass();
}

void DeviceDX12::PresentDevice()
{
	// Finish a command list
	// コマンドリストを終了する。
	efkRenderer->SetCommandList(nullptr);
	EffekseerRendererDX12::EndCommandList(efkCommandList);

	commandList->End();

	graphics->Execute(commandList);
	frames[nextFrame].Submitted = true;

	platform->Present();
	nextFrame = (nextFrame + 1) % frames.size();
}

void DeviceDX12::SetupEffekseerModules(::Effekseer::ManagerRef efkManager, bool usingProfiler)
{
	// Create a  graphics device
	// 描画デバイスの作成
	::Effekseer::Backend::GraphicsDeviceRef graphicsDevice;
	graphicsDevice = ::EffekseerRendererDX12::CreateGraphicsDevice(GetID3D12Device(), GetCommandQueue(), static_cast<int32_t>(frames.size()));

	// Create a renderer of effects
	// エフェクトのレンダラーの作成
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
	efkRenderer = ::EffekseerRendererDX12::Create(graphicsDevice, &format, 1, DXGI_FORMAT_UNKNOWN, false, 8000);
	if (efkRenderer == nullptr)
	{
		return;
	}

	// Create a memory pool
	// メモリプールの作成
	efkMemoryPool = EffekseerRenderer::CreateSingleFrameMemoryPool(efkRenderer->GetGraphicsDevice());

	// Pair vertex buffers and descriptors with the native command list for each slot.
	for (auto& frame : frames)
	{
		frame.Effekseer = EffekseerRenderer::CreateCommandList(efkRenderer->GetGraphicsDevice(), efkMemoryPool);
	}

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

	if (usingProfiler)
	{
		efkManager->SetGpuTimer(efkRenderer->CreateGpuTimer());
	}
}
