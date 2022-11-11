#pragma once

#include "../3rdParty/LLGI/src/LLGI.Base.h"
#include "../3rdParty/LLGI/src/LLGI.Graphics.h"
#include "../3rdParty/LLGI/src/Utils/LLGI.CommandListPool.h"

#include "EffectPlatform.h"

#if defined(WIN32) || defined(__APPLE__) || defined(__linux__)

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32 1
#endif

#ifdef __APPLE__
#define GLFW_EXPOSE_NATIVE_COCOA 1
#endif

#ifdef __linux__
#define GLFW_EXPOSE_NATIVE_X11 1
#undef Always
#endif

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#ifdef __linux__
#undef Always
#endif

class LLGIWindow : public LLGI::Window
{
	GLFWwindow* window_ = nullptr;

public:
	LLGIWindow(GLFWwindow* window)
		: window_(window)
	{
	}

	bool OnNewFrame() override
	{
		return glfwWindowShouldClose(window_) == GL_FALSE;
	}

	void* GetNativePtr(int32_t index) override
	{
#ifdef _WIN32
		if (index == 0)
		{
			return glfwGetWin32Window(window_);
		}

		return (HINSTANCE)GetModuleHandle(0);
#endif

#ifdef __APPLE__
		return glfwGetCocoaWindow(window_);
#endif

#ifdef __linux__
		if (index == 0)
		{
			return glfwGetX11Display();
		}

		return reinterpret_cast<void*>(glfwGetX11Window(window_));
#endif
	}

	LLGI::Vec2I GetWindowSize() const override
	{
		int w, h;
		glfwGetWindowSize(window_, &w, &h);
		return LLGI::Vec2I(w, h);
	}
};

#endif

class EffectPlatformLLGI : public EffectPlatform
{
private:
#if defined(WIN32) || defined(__APPLE__) || defined(__linux__)
	GLFWwindow* glfwWindow_ = nullptr;
#endif
protected:
	struct SimpleVertex
	{
		LLGI::Vec3F Pos;
		LLGI::Vec2F UV;
		LLGI::Color8 Color;
	};

	virtual void CreateShaders()
	{
	}
	void CreateResources();
	void CreateCheckedTexture();

	LLGI::Window* llgiWindow_ = nullptr;

	LLGI::Platform* platform_ = nullptr;
	LLGI::Graphics* graphics_ = nullptr;
	LLGI::SingleFrameMemoryPool* sfMemoryPool_ = nullptr;
	std::shared_ptr<LLGI::CommandList> commandList_ = nullptr;
	std::shared_ptr<LLGI::CommandListPool> commandListPool_ = nullptr;

	Effekseer::RefPtr<EffekseerRenderer::CommandList> commandListEfk_ = nullptr;
	Effekseer::RefPtr<EffekseerRenderer::SingleFrameMemoryPool> sfMemoryPoolEfk_ = nullptr;

	LLGI::RenderPass* renderPass_ = nullptr;
	LLGI::Texture* colorBuffer_ = nullptr;
	LLGI::Texture* depthBuffer_ = nullptr;
	LLGI::Shader* shader_vs_ = nullptr;
	LLGI::Shader* shader_ps_ = nullptr;
	LLGI::Buffer* vb_ = nullptr;
	LLGI::Buffer* ib_ = nullptr;
	LLGI::PipelineState* pip_ = nullptr;
	LLGI::PipelineState* screenPip_ = nullptr;
	LLGI::RenderPassPipelineState* rppip_ = nullptr;
	LLGI::Texture* checkTexture_ = nullptr;
	LLGI::TextureFormatType screenFormat_ = LLGI::TextureFormatType::R8G8B8A8_UNORM;
	LLGI::DeviceType deviceType_;

	void InitializeWindow() override;
	void Present() override;
	bool DoEvent() override;
	void PreDestroyDevice() override;
	void DestroyDevice() override;

public:
	EffectPlatformLLGI(LLGI::DeviceType deviceType);
	virtual ~EffectPlatformLLGI();

	virtual void BeginRendering() override;
	virtual void EndRendering() override;

	bool TakeScreenshot(const char* path) override;

	LLGI::Graphics* GetGraphics() const
	{
		return graphics_;
	}
};
