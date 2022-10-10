#include "GraphicsDevice.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#ifdef _WIN32
#include "Platform/DX11/efk.GraphicsDX11.h"
#endif
#include "Platform/GL/efk.GraphicsGL.h"

namespace Effekseer::Tool
{

bool GraphicsDevice::Initialize(void* handle, int width, int height, bool isSRGBMode, Effekseer::Tool::DeviceType deviceType)
{
	spdlog::trace("GraphicsWindow::Initialize : Begin");
	isSRGBMode_ = isSRGBMode;
	deviceType_ = deviceType;

	// because internal buffer is 16bit
	int32_t spriteCount = 65000 / 4;

	if (deviceType == Effekseer::Tool::DeviceType::OpenGL)
	{
		graphics_ = std::make_shared<efk::GraphicsGL>();
	}
#ifdef _WIN32
	else if (deviceType == Effekseer::Tool::DeviceType::DirectX11)
	{
		graphics_ = std::make_shared<efk::GraphicsDX11>();
	}
	else
	{
		assert(0);
	}
#endif

	spdlog::trace("OK new ::efk::Graphics");

	if (!graphics_->Initialize(handle, width, height))
	{
		spdlog::trace("Graphics::Initialize(false)");
		graphics_.reset();
		return false;
	}

	spdlog::trace("GraphicsWindow::Initialize : End");
	return true;
}

void GraphicsDevice::Resize(int32_t width, int32_t height)
{
	graphics_->Resize(width, height);
}

void GraphicsDevice::ResetRenderTargets()
{
	graphics_->SetRenderTarget({nullptr}, nullptr);
}

void GraphicsDevice::ClearColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	graphics_->Clear({r, g, b, a});
}

void GraphicsDevice::Present()
{
	graphics_->Present();
}

std::shared_ptr<GraphicsDevice> GraphicsDevice::Create(void* handle, int width, int height, bool isSRGBMode, Effekseer::Tool::DeviceType deviceType)
{
	auto ret = std::make_shared<GraphicsDevice>();
	if (ret->Initialize(handle, width, height, isSRGBMode, deviceType))
	{
		return ret;
	}

	return nullptr;
}

} // namespace Effekseer::Tool