#include "EffekseerRendererGL.DeviceObjectCollection.h"
#include "EffekseerRendererGL.DeviceObject.h"

namespace EffekseerRendererGL
{

void GraphicsDevice::Register(DeviceObject* device)
{
	deviceObjects_.insert(device);
}

void GraphicsDevice::Unregister(DeviceObject* device)
{
	deviceObjects_.erase(device);
}

void GraphicsDevice::OnLostDevice()
{
	for (auto& device : deviceObjects_)
	{
		device->OnLostDevice();
	}
}

void GraphicsDevice::OnResetDevice()
{
	for (auto& device : deviceObjects_)
	{
		device->OnResetDevice();
	}
}

} // namespace EffekseerRendererGL
