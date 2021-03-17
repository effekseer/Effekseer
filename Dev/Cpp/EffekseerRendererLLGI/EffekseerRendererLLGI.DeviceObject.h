
#ifndef __EFFEKSEERRENDERER_LLGI_DEVICEOBJECT_H__
#define __EFFEKSEERRENDERER_LLGI_DEVICEOBJECT_H__

#include "EffekseerRendererLLGI.RendererImplemented.h"

namespace EffekseerRendererLLGI
{

class DeviceObject
{
private:
	Backend::GraphicsDevice* graphicsDevice_ = nullptr;

	//! whether does this instance inc and dec the reference count of renderer
	bool hasRefCount_ = false;

public:
	DeviceObject(Backend::GraphicsDevice* graphicsDevice, bool hasRefCount);
	virtual ~DeviceObject();

public:
	Backend::GraphicsDevice* GetGraphicsDevice() const;

	virtual void OnLostDevice()
	{
	}
	virtual void OnResetDevice()
	{
	}
};

} // namespace EffekseerRendererLLGI

#endif // __EFFEKSEERRENDERER_LLGI_DEVICEOBJECT_H__
