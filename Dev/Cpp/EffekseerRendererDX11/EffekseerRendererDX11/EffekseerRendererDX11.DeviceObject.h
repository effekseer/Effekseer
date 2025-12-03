
#ifndef __EFFEKSEERRENDERER_DX11_DEVICEOBJECT_H__
#define __EFFEKSEERRENDERER_DX11_DEVICEOBJECT_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererDX11.RendererImplemented.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace EffekseerRendererDX11
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

class DeviceObject
{
private:
	RendererImplemented* renderer_;

	//! whether does this instance inc and dec the reference count of renderer
	bool has_ref_count_ = false;

public:
	DeviceObject(RendererImplemented* renderer, bool has_ref_count);
	virtual ~DeviceObject();

public:
	virtual void OnLostDevice() = 0;
	virtual void OnResetDevice() = 0;
	RendererImplemented* GetRenderer() const;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRendererDX11
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_DX11_DEVICEOBJECT_H__