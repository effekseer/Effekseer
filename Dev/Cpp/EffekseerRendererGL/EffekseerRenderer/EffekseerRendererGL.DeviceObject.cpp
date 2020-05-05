
#include "EffekseerRendererGL.DeviceObject.h"
#include "EffekseerRendererGL.DeviceObjectCollection.h"
#include "EffekseerRendererGL.RendererImplemented.h"

namespace EffekseerRendererGL
{

DeviceObject::DeviceObject(RendererImplemented* renderer, GraphicsDevice* graphicsDevice, bool hasRefCount)
	: renderer_(renderer), graphicsDevice_(graphicsDevice), hasRefCount_(hasRefCount)
{
	if (hasRefCount_)
	{
		ES_SAFE_ADDREF(renderer_);
		ES_SAFE_ADDREF(graphicsDevice_);	
	}

	graphicsDevice_->Register(this);
}

DeviceObject::~DeviceObject()
{
	graphicsDevice_->Unregister(this);

	if (hasRefCount_)
	{
		ES_SAFE_RELEASE(renderer_);
		ES_SAFE_RELEASE(graphicsDevice_);
	}
}

RendererImplemented* DeviceObject::GetRenderer() const { return renderer_; }

} // namespace EffekseerRendererGL
