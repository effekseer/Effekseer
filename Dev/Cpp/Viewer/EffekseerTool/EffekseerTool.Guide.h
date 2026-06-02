#pragma once

#include <EffekseerToolRuntime/ImageRenderer.h>

namespace EffekseerRenderer
{

class Guide
{
private:
	std::shared_ptr<Effekseer::ToolRuntime::ImageRenderer> imageRenderer_ = nullptr;

	Guide(Effekseer::Backend::GraphicsDeviceRef graphicsDevice);

public:
	virtual ~Guide();

	static Guide* Create(Effekseer::Backend::GraphicsDeviceRef graphicsDevice);

public:
	void Rendering(int32_t width, int32_t height, int32_t guide_width, int32_t guide_height);
};

} // namespace EffekseerRenderer
