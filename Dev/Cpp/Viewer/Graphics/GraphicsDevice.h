#pragma once

#include "../Math/Vector3F.h"
#include "../Parameters.h"
#include "efk.Graphics.h"
#include <Effekseer.h>

namespace Effekseer::Tool
{

class GraphicsDevice
{
	std::shared_ptr<efk::Graphics> graphics_ = nullptr;
	DeviceType deviceType_;
	bool isSRGBMode_ = false;

	bool Initialize(void* handle, int width, int height, bool isSRGBMode, DeviceType deviceType);

public:
	void Resize(int32_t width, int32_t height);

	void ClearColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

	void Present();

	static std::shared_ptr<GraphicsDevice> Create(void* handle, int width, int height, bool isSRGBMode, DeviceType deviceType);

#ifndef SWIG

	DeviceType GetDeviceType() const
	{
		return deviceType_;
	}

	bool GetIsSRGBMode() const
	{
		return isSRGBMode_;
	}

	std::shared_ptr<efk::Graphics> GetGraphics() const
	{
		return graphics_;
	}
#endif
};

} // namespace Effekseer::Tool