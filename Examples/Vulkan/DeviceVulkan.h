#pragma once

#include <vector>

#include "../Utils/Input.h"
#include "../Utils/Window.h"
#include <EffekseerRendererVulkan.h>
#include <LLGI.Compiler.h>
#include <LLGI.Graphics.h>
#include <LLGI.Platform.h>
#include <Vulkan/LLGI.CommandListVulkan.h>
#include <Vulkan/LLGI.GraphicsVulkan.h>

class DeviceVulkan
{
private:
	std::shared_ptr<LLGI::Window> window;
	std::shared_ptr<LLGI::Platform> platform;
	std::shared_ptr<LLGI::Graphics> graphics;
	std::shared_ptr<LLGI::SingleFrameMemoryPool> memoryPool;
	struct FrameResources
	{
		std::shared_ptr<LLGI::CommandList> Native;
		::Effekseer::RefPtr<EffekseerRenderer::CommandList> Effekseer;
		bool Submitted = false;
	};
	std::vector<FrameResources> frames;
	size_t nextFrame = 0;
	LLGI::CommandList* commandList = nullptr;

	::EffekseerRenderer::RendererRef efkRenderer;
	::Effekseer::RefPtr<EffekseerRenderer::SingleFrameMemoryPool> efkMemoryPool;
	::Effekseer::RefPtr<EffekseerRenderer::CommandList> efkCommandList;

public:
	DeviceVulkan() = default;
	~DeviceVulkan()
	{
		Terminate();
	}

	Utils::Vec2I GetWindowSize() const
	{
		auto size = window->GetWindowSize();
		return {size.X, size.Y};
	}

	VkPhysicalDevice GetVkPhysicalDevice()
	{
		return static_cast<VkPhysicalDevice>(static_cast<LLGI::GraphicsVulkan*>(graphics.get())->GetPysicalDevice());
	}

	VkDevice GetVkDevice()
	{
		return static_cast<VkDevice>(static_cast<LLGI::GraphicsVulkan*>(graphics.get())->GetDevice());
	}

	VkQueue GetVkQueue()
	{
		return static_cast<VkQueue>(static_cast<LLGI::GraphicsVulkan*>(graphics.get())->GetQueue());
	}

	VkCommandPool GetVkCommandPool()
	{
		return static_cast<VkCommandPool>(static_cast<LLGI::GraphicsVulkan*>(graphics.get())->GetCommandPool());
	}

	VkCommandBuffer GetCommandList()
	{
		return static_cast<VkCommandBuffer>(static_cast<LLGI::CommandListVulkan*>(commandList)->GetCommandBuffer());
	}

	int GetSwapBufferCount()
	{
		return static_cast<int>(frames.size());
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
