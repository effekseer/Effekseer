

#include "EffekseerRendererLLGI.DeviceObject.h"
#include "EffekseerRendererLLGI.RendererImplemented.h"

namespace EffekseerRendererLLGI
{

DeviceObject::DeviceObject(GraphicsDevice* graphicsDevice, bool hasRefCount)
	: graphicsDevice_(graphicsDevice)
{
	if (hasRefCount_)
	{
		ES_SAFE_ADDREF(graphicsDevice_);
	}

	graphicsDevice_->Register(this);
}

DeviceObject::~DeviceObject()
{
	graphicsDevice_->Unregister(this);

	if (hasRefCount_)
	{
		ES_SAFE_RELEASE(graphicsDevice_);
	}
}

GraphicsDevice* DeviceObject::GetGraphicsDevice() const
{
	return graphicsDevice_;
}

} // namespace EffekseerRendererLLGI
