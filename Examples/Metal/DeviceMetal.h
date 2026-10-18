#pragma once

#include "../Utils/Input.h"
#include "../Utils/Window.h"
#include <EffekseerRendererMetal.h>
#include <LLGI.Compiler.h>
#include <LLGI.Graphics.h>
#include <LLGI.Platform.h>
#include <Metal/LLGI.CommandListMetal.h>
#include <Metal/LLGI.GraphicsMetal.h>
#include <Utils/LLGI.CommandListPool.h>

class DeviceMetal
{
private:
	std::shared_ptr<LLGI::Window> window;
	std::shared_ptr<LLGI::Platform> platform;
	std::shared_ptr<LLGI::Graphics> graphics;
	std::shared_ptr<LLGI::SingleFrameMemoryPool> memoryPool;
	std::shared_ptr<LLGI::CommandListPool> commandListPool;
	LLGI::CommandList* commandList = nullptr;

	::EffekseerRenderer::RendererRef efkRenderer;
	::Effekseer::RefPtr<EffekseerRenderer::SingleFrameMemoryPool> efkMemoryPool;
	::Effekseer::RefPtr<EffekseerRenderer::CommandList> efkCommandList;

public:
	DeviceMetal() = default;
	~DeviceMetal()
	{
		Terminate();
	}

	Utils::Vec2I GetWindowSize() const
	{
		auto size = window->GetWindowSize();
		return {size.X, size.Y};
	}

	id<MTLRenderCommandEncoder> GetRenderEncoder()
	{
		return static_cast<LLGI::CommandListMetal*>(commandList)->GetRenderCommandEncorder();
	}
	id<MTLComputeCommandEncoder> GetComputeEncoder()
	{
		return static_cast<LLGI::CommandListMetal*>(commandList)->GetComputeCommandEncorder();
	}

	bool Initialize(const char* windowTitle, Utils::Vec2I windowSize);
	void Terminate();
	bool NewFrame();
	void BeginComputePass();
	void EndComputePass();
	void BeginRenderPass();
	void EndRenderPass();
	void PresentDevice();

	void SetupEffekseerModules(::Effekseer::ManagerRef efkManager, bool usingProfiler = false);
	::EffekseerRenderer::RendererRef GetEffekseerRenderer()
	{
		return efkRenderer;
	}
};
