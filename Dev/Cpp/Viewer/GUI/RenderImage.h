#pragma once

#include "../Graphics/GraphicsDevice.h"
#include "Image.h"
#include <EffekseerToolRuntime/RenderImage.h>

namespace Effekseer::Tool
{

class RenderImage : public Effekseer::ToolRuntime::RenderImage
{
public:
	//! dummy
	RenderImage() = default;

#if !defined(SWIG)
	RenderImage(Effekseer::Backend::GraphicsDeviceRef graphicsDevice);
#endif

	static std::shared_ptr<Effekseer::Tool::RenderImage> Create(std::shared_ptr<GraphicsDevice> graphicsDevice);
};

} // namespace Effekseer::Tool
