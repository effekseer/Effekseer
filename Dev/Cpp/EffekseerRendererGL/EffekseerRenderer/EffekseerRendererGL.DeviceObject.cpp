
#include "EffekseerRendererGL.DeviceObject.h"
#include "EffekseerRendererGL.RendererImplemented.h"

namespace EffekseerRendererGL
{

DeviceObject::DeviceObject(Backend::GraphicsDevice* graphicsDevice)
	: graphicsDevice_(graphicsDevice)
{
	ES_SAFE_ADDREF(graphicsDevice_);

	graphicsDevice_->Register(this);
}

DeviceObject::~DeviceObject()
{
	graphicsDevice_->Unregister(this);

	ES_SAFE_RELEASE(graphicsDevice_);
}

} // namespace EffekseerRendererGL
