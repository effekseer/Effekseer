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

	window = std::shared_ptr<LLGI::Window>(LLGI::CreateWindow(windowTitle, { windowSize.X, windowSize.Y }));
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

	// Release COM
	// COMの解放
	CoUninitialize();
}

void DeviceDX12::ClearScreen()
{
}

bool DeviceDX12::OnNewFrame()
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
	commandList->BeginRenderPass(
		platform->GetCurrentScreen(color, true, false)); // TODO: isDepthClear is false, because it fails with dx12.

	return true;
}

void DeviceDX12::PresentDevice()
{
	commandList->EndRenderPass();
	commandList->End();

	graphics->Execute(commandList);

	platform->Present();
}
