
#ifndef __EFFEKSEERRENDERER_DX9_DEVICEOBJECT_H__
#define __EFFEKSEERRENDERER_DX9_DEVICEOBJECT_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererDX9.RendererImplemented.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace EffekseerRendererDX9
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	デバイスによって生成されるオブジェクト
*/
class DeviceObject
{
private:
	RendererImplemented* m_renderer = nullptr;

	//! whether does this instance inc and dec the reference count of renderer
	bool hasRefCount_ = false;

public:
	DeviceObject(RendererImplemented* renderer, bool hasRefCount);
	virtual ~DeviceObject();

public:
	virtual void OnLostDevice() = 0;
	virtual void OnResetDevice() = 0;
	virtual void OnChangeDevice()
	{
	}
	RendererImplemented* GetRenderer() const;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRendererDX9
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_DX9_DEVICEOBJECT_H__