
#ifndef	__EFFEKSEERRENDERER_RENDERSTATE_H__
#define	__EFFEKSEERRENDERER_RENDERSTATE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "../../EffekseerRendererCommon/EffekseerRenderer.RenderStateBase.h"
#include "EffekseerRenderer.Base.h"
#include "EffekseerRenderer.Renderer.h"

//#define __USE_SAMPLERS	1
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
class RenderState
	: public RenderStateBase
{
private:
	RendererImplemented*	m_renderer;

#ifdef	__USE_SAMPLERS
	GLuint					m_samplers[4];
#endif

public:
	RenderState( RendererImplemented* renderer );
	virtual ~RenderState();

	void Update( bool forced );
};

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
}
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRENDERER_RENDERSTATE_H__