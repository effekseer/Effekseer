#pragma once

#include "Image.h"

namespace Effekseer::ToolRuntime
{

class RenderImage : public Image
{
private:
	Effekseer::Backend::GraphicsDeviceRef graphicsDevice_;

public:
	//! dummy
	RenderImage() = default;

#if !defined(SWIG)
	RenderImage(Effekseer::Backend::GraphicsDeviceRef graphicsDevice);
#endif

	void Resize(int32_t width, int32_t height);

	static std::shared_ptr<Effekseer::ToolRuntime::RenderImage> Create(Effekseer::Backend::GraphicsDeviceRef graphicsDevice);
};

} // namespace Effekseer::ToolRuntime
