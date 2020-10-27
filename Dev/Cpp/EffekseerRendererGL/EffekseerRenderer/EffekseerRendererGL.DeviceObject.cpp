
#include "EffekseerRendererGL.DeviceObject.h"
#include "EffekseerRendererGL.DeviceObjectCollection.h"
#include "EffekseerRendererGL.RendererImplemented.h"

namespace EffekseerRendererGL
{

DeviceObject::DeviceObject(RendererImplemented* renderer, DeviceObjectCollection* deviceObjectCollection, bool hasRefCount)
	: renderer_(renderer)
	, deviceObjectCollection_(deviceObjectCollection)
	, hasRefCount_(hasRefCount)
{
	if (hasRefCount_)
	{
		ES_SAFE_ADDREF(renderer_);
		ES_SAFE_ADDREF(deviceObjectCollection_);
	}

	deviceObjectCollection_->Register(this);
}

DeviceObject::~DeviceObject()
{
	deviceObjectCollection_->Unregister(this);

	if (hasRefCount_)
	{
		ES_SAFE_RELEASE(renderer_);
		ES_SAFE_RELEASE(deviceObjectCollection_);
	}
}

RendererImplemented* DeviceObject::GetRenderer() const
{
	return renderer_;
}

} // namespace EffekseerRendererGL
