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
	LLGIWindow(GLFWwindow* window) : window_(window) {}

	bool OnNewFrame() override { return glfwWindowShouldClose(window_) == GL_FALSE; }

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
	LLGI::Window* llgiWindow_ = nullptr;

	LLGI::Platform* platform_ = nullptr;
	LLGI::Graphics* graphics_ = nullptr;
	LLGI::SingleFrameMemoryPool* sfMemoryPool_ = nullptr;
	LLGI::CommandList* commandList_ = nullptr;
	std::shared_ptr<LLGI::CommandListPool> commandListPool_ = nullptr;

	void Present() override;
	bool DoEvent() override;
	void PreDestroyDevice() override;
	void DestroyDevice() override;

public:
	EffectPlatformLLGI();
	virtual ~EffectPlatformLLGI();
	bool TakeScreenshot(const char* path);

	LLGI::Graphics* GetGraphics() const { return graphics_; }
};