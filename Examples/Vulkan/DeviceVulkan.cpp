#include "DeviceVulkan.h"

#include <LLGI.Texture.h>

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
	frames.resize(platform->GetMaxFrameCount());
	for (auto& frame : frames)
	{
		frame.Native = LLGI::CreateSharedPtr(graphics->CreateCommandList(memoryPool.get()));
	}

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
	frames.clear();
	commandList = nullptr;
	nextFrame = 0;
	efkMemoryPool.Reset();
	efkRenderer.Reset();

	memoryPool.reset();
	graphics.reset();
	platform.reset();
	window.reset();
}

bool DeviceVulkan::NewFrame()
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
	EffekseerRendererVulkan::BeginCommandList(efkCommandList, GetCommandList());
	efkRenderer->SetCommandList(efkCommandList);

#if defined(_WIN32)
	bool windowActive = GetActiveWindow() == window->GetNativePtr(0);
	for (int key = 0; key < 256; key++)
	{
		Utils::Input::UpdateKeyState(key, windowActive && (GetAsyncKeyState(key) & 0x01) != 0);
	}
#endif

	return true;
}

void DeviceVulkan::BeginComputePass()
{
	commandList->BeginComputePass();
}

void DeviceVulkan::EndComputePass()
{
	commandList->EndComputePass();
}

void DeviceVulkan::BeginRenderPass()
{
	LLGI::Color8 color{0, 0, 0, 255};
	commandList->BeginRenderPass(platform->GetCurrentScreen(color, true, true));
}

void DeviceVulkan::EndRenderPass()
{
	commandList->EndRenderPass();
}

void DeviceVulkan::PresentDevice()
{
	// Finish a command list
	// コマンドリストを終了する。
	efkRenderer->SetCommandList(nullptr);
	EffekseerRendererVulkan::EndCommandList(efkCommandList);

	commandList->End();

	graphics->Execute(commandList);
	frames[nextFrame].Submitted = true;

	platform->Present();
	nextFrame = (nextFrame + 1) % frames.size();
}

void DeviceVulkan::SetupEffekseerModules(::Effekseer::ManagerRef efkManager, bool usingProfiler)
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
	auto screenRenderPass = platform->GetCurrentScreen();
	if (screenRenderPass != nullptr && screenRenderPass->GetHasDepthTexture())
	{
		auto depthFormat = screenRenderPass->GetDepthTexture()->GetFormat();
		renderPassInfo.DepthFormat = LLGI::VulkanHelper::TextureFormatToVkFormat(depthFormat);
	}
	efkRenderer = ::EffekseerRendererVulkan::Create(graphicsDevice, renderPassInfo, 8000);
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
