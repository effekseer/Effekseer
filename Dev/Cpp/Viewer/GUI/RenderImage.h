#pragma once

#include "../Graphics/GraphicsDevice.h"
#include "Image.h"

namespace Effekseer::Tool
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

	static std::shared_ptr<Effekseer::Tool::RenderImage> Create(std::shared_ptr<GraphicsDevice> graphicsDevice);
};

} // namespace Effekseer::Tool