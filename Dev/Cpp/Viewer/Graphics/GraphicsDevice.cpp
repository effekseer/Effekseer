#include "GraphicsDevice.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#ifdef _WIN32
#include "Platform/DX11/efk.GraphicsDX11.h"
#endif
#include "Platform/GL/efk.GraphicsGL.h"

namespace Effekseer::Tool
{

bool GraphicsDevice::Initialize(void* handle, int width, int height, bool isSRGBMode, efk::DeviceType deviceType)
{
	spdlog::trace("GraphicsWindow::Initialize : Begin");
	isSRGBMode_ = isSRGBMode;
	deviceType_ = deviceType;

	// because internal buffer is 16bit
	int32_t spriteCount = 65000 / 4;

	if (deviceType == efk::DeviceType::OpenGL)
	{
		graphics_ = std::make_shared<efk::GraphicsGL>();
	}
#ifdef _WIN32
	else if (deviceType == efk::DeviceType::DirectX11)
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

std::shared_ptr<GraphicsDevice> GraphicsDevice::Create(void* handle, int width, int height, bool isSRGBMode, efk::DeviceType deviceType)
{
	auto ret = std::make_shared<GraphicsDevice>();
	if (ret->Initialize(handle, width, height, isSRGBMode, deviceType))
	{
		return ret;
	}

	return nullptr;
}

} // namespace Effekseer::Tool