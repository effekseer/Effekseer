#pragma once

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
};

} // namespace Effekseer::Tool