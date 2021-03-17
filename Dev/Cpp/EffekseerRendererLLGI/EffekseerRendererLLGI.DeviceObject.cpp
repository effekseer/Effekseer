

#include "EffekseerRendererLLGI.DeviceObject.h"
#include "EffekseerRendererLLGI.RendererImplemented.h"

namespace EffekseerRendererLLGI
{

DeviceObject::DeviceObject(Backend::GraphicsDevice* graphicsDevice, bool hasRefCount)
	: graphicsDevice_(graphicsDevice)
{
	if (hasRefCount_)
	{
		ES_SAFE_ADDREF(graphicsDevice_);
	}
}

DeviceObject::~DeviceObject()
{
	if (hasRefCount_)
	{
		ES_SAFE_RELEASE(graphicsDevice_);
	}
}

Backend::GraphicsDevice* DeviceObject::GetGraphicsDevice() const
{
	return graphicsDevice_;
}

} // namespace EffekseerRendererLLGI
