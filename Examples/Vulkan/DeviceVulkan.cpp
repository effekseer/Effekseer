#include "DeviceVulkan.h"

bool DeviceVulkan::Initialize(const char* windowTitle, Utils::Vec2I windowSize)
{
	// A code to initialize DirectX12 is too long, so I use LLGI
	// Vulkan初期化のためのコードは長すぎるのでLLGIを使用する。

	LLGI::PlatformParameter platformParam{};
	platformParam.Device = LLGI::DeviceType::Vulkan;
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

	return true;
}

void DeviceVulkan::Terminate()
{
	// Release Vulkan
	// Vulkanの解放

	if (graphics)
	{
		graphics->WaitFinish();
	}

	efkCommandList.Reset();
	efkMemoryPool.Reset();
	efkRenderer.Reset();

	commandListPool.reset();
	memoryPool.reset();
	graphics.reset();
	platform.reset();
	window.reset();
}

void DeviceVulkan::ClearScreen()
{
}

bool DeviceVulkan::NewFrame()
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
	commandList->BeginRenderPass(platform->GetCurrentScreen(color, true, true));

	// Call on starting of a frame
	// フレームの開始時に呼ぶ
	efkMemoryPool->NewFrame();

	// Begin a command list
	// コマンドリストを開始する。
	EffekseerRendererVulkan::BeginCommandList(efkCommandList, GetCommandList());
	efkRenderer->SetCommandList(efkCommandList);

	return true;
}

void DeviceVulkan::PresentDevice()
{
	// Finish a command list
	// コマンドリストを終了する。
	efkRenderer->SetCommandList(nullptr);
	EffekseerRendererVulkan::EndCommandList(efkCommandList);

	commandList->EndRenderPass();
	commandList->End();

	graphics->Execute(commandList);

	platform->Present();
}

void DeviceVulkan::SetupEffekseerModules(::Effekseer::ManagerRef efkManager)
{
	// Create a  graphics device
	// 描画デバイスの作成
	auto graphicsDevice = ::EffekseerRendererVulkan::CreateGraphicsDevice(
		GetVkPhysicalDevice(),
		GetVkDevice(),
		GetVkQueue(),
		GetVkCommandPool(),
		GetSwapBufferCount());

	// Create a renderer of effects
	// エフェクトのレンダラーの作成
	::EffekseerRendererVulkan::RenderPassInformation renderPassInfo;
	renderPassInfo.DoesPresentToScreen = true;
	renderPassInfo.RenderTextureCount = 1;
	renderPassInfo.RenderTextureFormats[0] = VK_FORMAT_B8G8R8A8_UNORM;
	renderPassInfo.DepthFormat = VK_FORMAT_D24_UNORM_S8_UINT;
	efkRenderer = ::EffekseerRendererVulkan::Create(graphicsDevice, renderPassInfo, 8000);

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
