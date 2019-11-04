#include "EffectPlatformLLGI.h"
#include "../3rdParty/LLGI/src/LLGI.CommandList.h"
#include "../3rdParty/LLGI/src/LLGI.Platform.h"

#include "../../3rdParty/stb/stb_image_write.h"

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
	commandListPool_ = std::make_shared<LLGI::CommandListPool>(graphics_, sfMemoryPool_, 3);
}

EffectPlatformLLGI ::~EffectPlatformLLGI() {}

void EffectPlatformLLGI::Present()
{
	graphics_->Execute(commandList_);
	platform_->Present();
}

bool EffectPlatformLLGI::DoEvent()
{
	if (!platform_->NewFrame())
		return false;

	glfwPollEvents();

	sfMemoryPool_->NewFrame();
	commandList_ = commandListPool_->Get();

	return true;
}

void EffectPlatformLLGI::PreDestroyDevice() { graphics_->WaitFinish(); }

void EffectPlatformLLGI::DestroyDevice()
{
	LLGI::SafeRelease(sfMemoryPool_);
	commandListPool_.reset();
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

bool EffectPlatformLLGI::TakeScreenshot(const char* path)
{
	commandList_->WaitUntilCompleted();

	LLGI::Color8 color;
	color.R = 255;
	color.G = 255;
	color.B = 255;
	color.A = 255;

	auto texture = platform_->GetCurrentScreen(color, true)->GetRenderTexture(0);
	auto data = graphics_->CaptureRenderTarget(texture);

	stbi_write_png(path, 1280, 720, 4, data.data(), 1280 * 4);

	return true;
}