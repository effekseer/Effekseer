
#ifndef	__EFFEKSEERRENDERER_GL_DEVICEOBJECT_H__
#define	__EFFEKSEERRENDERER_GL_DEVICEOBJECT_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererGL.RendererImplemented.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace EffekseerRendererGL
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
	RendererImplemented*	m_renderer;

public:
		DeviceObject( RendererImplemented* renderer );
		virtual ~DeviceObject();

	public:
		virtual void OnLostDevice() = 0;
		virtual void OnResetDevice() = 0;

		RendererImplemented* GetRenderer() const;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRENDERER_GL_DEVICEOBJECT_H__