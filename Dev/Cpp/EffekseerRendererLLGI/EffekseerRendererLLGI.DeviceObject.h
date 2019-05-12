
#ifndef __EFFEKSEERRENDERER_LLGI_DEVICEOBJECT_H__
#define __EFFEKSEERRENDERER_LLGI_DEVICEOBJECT_H__

#include "EffekseerRendererLLGI.RendererImplemented.h"

namespace EffekseerRendererLLGI
{

/**
	@brief	デバイスによって生成されるオブジェクト
*/
class DeviceObject
{
private:
	RendererImplemented* m_renderer;

public:
	DeviceObject(RendererImplemented* renderer);
	virtual ~DeviceObject();

public:
	RendererImplemented* GetRenderer() const;
};

} // namespace EffekseerRendererLLGI

#endif // __EFFEKSEERRENDERER_LLGI_DEVICEOBJECT_H__
