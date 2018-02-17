
#ifndef	__EFFEKSEERRENDERER_CULLING_H__
#define	__EFFEKSEERRENDERER_CULLING_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <EffekseerRenderer/EffekseerRendererDX9.Renderer.h>
#include <EffekseerRenderer/EffekseerRendererDX9.RendererImplemented.h>
#include <EffekseerRenderer/EffekseerRendererDX9.DeviceObject.h>

#include "../Graphics/Platform/DX9/efk.LineRendererDX9.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class Culling
	: public EffekseerRendererDX9::DeviceObject
{
private:

	EffekseerRendererDX9::RendererImplemented*			m_renderer;
	efk::LineRendererDX9*	lineRenderer = nullptr;


	Culling( EffekseerRendererDX9::RendererImplemented* renderer);
public:

	virtual ~Culling();

	static Culling* Create( EffekseerRendererDX9::RendererImplemented* renderer );

public:	// デバイス復旧用
	virtual void OnLostDevice();
	virtual void OnResetDevice();

public:
	void Rendering(bool isRightHand );

	bool IsShown;
	float Radius;
	float X;
	float Y;
	float Z;
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRENDERER_CULLING_H__