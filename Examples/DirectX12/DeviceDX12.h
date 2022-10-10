#pragma once

#include <DX12/LLGI.CommandListDX12.h>
#include <DX12/LLGI.GraphicsDX12.h>
#include <EffekseerRendererDX12.h>
#include <EffekseerSoundXAudio2.h>
#include <LLGI.Compiler.h>
#include <LLGI.Graphics.h>
#include <LLGI.Platform.h>
#include <Utils/LLGI.CommandListPool.h>
#include <XAudio2.h>
#include <d3d12.h>
#include <wrl/client.h>

#include "../Utils/Window.h"

class DeviceDX12
{
private:
	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	std::shared_ptr<LLGI::Window> window;
	std::shared_ptr<LLGI::Platform> platform;
	std::shared_ptr<LLGI::Graphics> graphics;
	std::shared_ptr<LLGI::SingleFrameMemoryPool> memoryPool;
	std::shared_ptr<LLGI::CommandListPool> commandListPool;
	LLGI::CommandList* commandList = nullptr;

	ComPtr<IXAudio2> xa2Device;
	IXAudio2MasteringVoice* xa2MasterVoice = nullptr;

	::EffekseerRenderer::RendererRef efkRenderer;
	::EffekseerSound::SoundRef efkSound;
	::Effekseer::RefPtr<EffekseerRenderer::SingleFrameMemoryPool> efkMemoryPool;
	::Effekseer::RefPtr<EffekseerRenderer::CommandList> efkCommandList;

public:
	DeviceDX12() = default;
	~DeviceDX12()
	{
		Terminate();
	}

	Utils::Vec2I GetWindowSize() const
	{
		auto size = window->GetWindowSize();
		return {size.X, size.Y};
	}

	ID3D12Device* GetID3D12Device()
	{
		assert(graphics != nullptr);
		return static_cast<LLGI::GraphicsDX12*>(graphics.get())->GetDevice();
	}

	ID3D12CommandQueue* GetCommandQueue()
	{
		assert(graphics != nullptr);
		return static_cast<LLGI::GraphicsDX12*>(graphics.get())->GetCommandQueue();
	}

	ID3D12GraphicsCommandList* GetCommandList()
	{
		assert(commandList != nullptr);
		return static_cast<LLGI::CommandListDX12*>(commandList)->GetCommandList();
	}

	IXAudio2* GetIXAudio2()
	{
		return xa2Device.Get();
	}

	bool Initialize(const char* windowTitle, Utils::Vec2I windowSize);
	void Terminate();
	void ClearScreen();
	bool NewFrame();
	void PresentDevice();

	void SetupEffekseerModules(::Effekseer::ManagerRef efkManager);
	::EffekseerRenderer::RendererRef GetEffekseerRenderer()
	{
		return efkRenderer;
	}
};
