
#ifndef	__EFFEKSEERRENDERER_GL_RENDERSTATE_H__
#define	__EFFEKSEERRENDERER_GL_RENDERSTATE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "../../EffekseerRendererCommon/EffekseerRenderer.RenderStateBase.h"
#include "EffekseerRendererGL.Base.h"
#include "EffekseerRendererGL.Renderer.h"

//#define __USE_SAMPLERS	1
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererGL
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
class RenderState
	: public ::EffekseerRenderer::RenderStateBase
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
#endif	// __EFFEKSEERRENDERER_GL_RENDERSTATE_H__