#pragma once

#include <Metal/LLGI.CommandListMetal.h>
#include <Metal/LLGI.GraphicsMetal.h>
#include <LLGI.Compiler.h>
#include <LLGI.Graphics.h>
#include <LLGI.Platform.h>
#include <Utils/LLGI.CommandListPool.h>
#include <EffekseerRendererMetal.h>
#include "../Utils/Window.h"

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
	~DeviceMetal() { Terminate(); }

	Utils::Vec2I GetWindowSize() const
	{
		auto size = window->GetWindowSize();
		return { size.X, size.Y };
	}

	id<MTLRenderCommandEncoder> GetEncoder()
	{
		return static_cast<LLGI::CommandListMetal*>(commandList)->GetRenderCommandEncorder();
	}

	bool Initialize(const char* windowTitle, Utils::Vec2I windowSize);
	void Terminate();
	void ClearScreen();
	bool NewFrame();
	void PresentDevice();

	void SetupEffekseerModules(::Effekseer::ManagerRef efkManager);
	::EffekseerRenderer::RendererRef GetEffekseerRenderer() { return efkRenderer; }
};
