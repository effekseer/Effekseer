
#ifndef __EFFEKSEERRENDERER_GL_DEVICEOBJECT_H__
#define __EFFEKSEERRENDERER_GL_DEVICEOBJECT_H__

#include "GraphicsDevice.h"
#include <stdint.h>
#include <stdio.h>

namespace EffekseerRendererGL
{

class DeviceObject : public Backend::DeviceObject
{
private:
	Backend::GraphicsDevice* graphicsDevice_ = nullptr;

public:
	DeviceObject(Backend::GraphicsDevice* graphicsDevice);
	virtual ~DeviceObject();

public:
	virtual void OnLostDevice() = 0;
	virtual void OnResetDevice() = 0;
};

} // namespace EffekseerRendererGL

#endif // __EFFEKSEERRENDERER_GL_DEVICEOBJECT_H__