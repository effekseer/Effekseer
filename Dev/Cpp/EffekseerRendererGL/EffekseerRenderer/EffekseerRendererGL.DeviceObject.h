
#ifndef __EFFEKSEERRENDERER_GL_DEVICEOBJECT_H__
#define __EFFEKSEERRENDERER_GL_DEVICEOBJECT_H__

#include <stdint.h>
#include <stdio.h>

namespace EffekseerRendererGL
{

class DeviceObjectCollection;
class RendererImplemented;

class DeviceObject
{
private:
	RendererImplemented* renderer_;
	DeviceObjectCollection* deviceObjectCollection_ = nullptr;

	//! whether does this instance inc and dec the reference count of renderer
	bool hasRefCount_ = false;

public:
	DeviceObject(RendererImplemented* renderer, DeviceObjectCollection* deviceObjectCollection, bool hasRefCount);
	virtual ~DeviceObject();

public:
	virtual void OnLostDevice() = 0;
	virtual void OnResetDevice() = 0;

	RendererImplemented* GetRenderer() const;
};

} // namespace EffekseerRendererGL

#endif // __EFFEKSEERRENDERER_GL_DEVICEOBJECT_H__