#include "EffekseerRendererGL.DeviceObjectCollection.h"
#include "EffekseerRendererGL.DeviceObject.h"

namespace EffekseerRendererGL
{

void DeviceObjectCollection::Register(DeviceObject* device)
{
	deviceObjects_.insert(device);
}

void DeviceObjectCollection::Unregister(DeviceObject* device)
{
	deviceObjects_.erase(device);
}

void DeviceObjectCollection::OnLostDevice()
{
	for (auto& device : deviceObjects_)
	{
		device->OnLostDevice();
	}
}

void DeviceObjectCollection::OnResetDevice()
{
	for (auto& device : deviceObjects_)
	{
		device->OnResetDevice();
	}
}

} // namespace EffekseerRendererGL
