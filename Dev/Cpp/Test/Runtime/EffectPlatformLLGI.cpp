#include "EffectPlatformLLGI.h"
#include "../3rdParty/LLGI/src/LLGI.CommandList.h"
#include "../3rdParty/LLGI/src/LLGI.Platform.h"

EffectPlatformLLGI::EffectPlatformLLGI()
{
	if (!glfwInit())
	{
		throw "Failed to initialize glfw";
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	glfwWindow_ = glfwCreateWindow(1280, 720, "Example glfw", nullptr, nullptr);

	if (glfwWindow_ == nullptr)
	{
		glfwTerminate();
		throw "Failed to create an window.";
	}

	llgiWindow_ = new LLGIWindow(glfwWindow_);

	platform_ = LLGI::CreatePlatform(LLGI::DeviceType::Default, llgiWindow_);
	graphics_ = platform_->CreateGraphics();
	sfMemoryPool_ = graphics_->CreateSingleFrameMemoryPool(1024 * 1024, 128);
	commandList_ = graphics_->CreateCommandList(sfMemoryPool_);
}

EffectPlatformLLGI ::~EffectPlatformLLGI()
{
	graphics_->WaitFinish();

	DestroyInternal();

	LLGI::SafeRelease(sfMemoryPool_);
	LLGI::SafeRelease(commandList_);
	LLGI::SafeRelease(graphics_);
	LLGI::SafeRelease(platform_);

	ES_SAFE_DELETE(llgiWindow_);

	if (glfwWindow_ != nullptr)
	{
		glfwDestroyWindow(glfwWindow_);
		glfwTerminate();
		glfwWindow_ = nullptr;
	}
}

void EffectPlatformLLGI::Present()
{
	graphics_->Execute(commandList_);
	platform_->Present();
}

bool EffectPlatformLLGI::DoEvent()
{
	if (glfwWindowShouldClose(glfwWindow_) == GL_TRUE)
	{
		return false;
	}

	glfwPollEvents();

	if (!platform_->NewFrame())
		return false;

	sfMemoryPool_->NewFrame();

	return true;
}

bool EffectPlatformLLGI::TakeScreenshot(const char* path) { return false; }