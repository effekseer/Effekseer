#pragma once

#include "../Math/Vector3F.h"
#include "efk.Graphics.h"
#include <Effekseer.h>

namespace Effekseer::Tool
{

class GraphicsDevice
{
	std::shared_ptr<efk::Graphics> graphics_ = nullptr;
	efk::DeviceType deviceType_;
	bool isSRGBMode_ = false;

	bool Initialize(void* handle, int width, int height, bool isSRGBMode, efk::DeviceType deviceType);

public:
	static std::shared_ptr<GraphicsDevice> Create(void* handle, int width, int height, bool isSRGBMode, efk::DeviceType deviceType);

#ifndef SWIG

	efk::DeviceType GetDeviceType() const
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