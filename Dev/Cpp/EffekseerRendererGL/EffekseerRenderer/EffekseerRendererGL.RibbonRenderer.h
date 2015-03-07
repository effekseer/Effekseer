
#ifndef	__EFFEKSEERRENDERER_GL_STRIPE_RENDERER_H__
#define	__EFFEKSEERRENDERER_GL_STRIPE_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererGL.Renderer.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.RibbonRendererBase.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererGL
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
typedef ::Effekseer::RibbonRenderer::NodeParameter efkRibbonNodeParam;
typedef ::Effekseer::RibbonRenderer::InstanceParameter efkRibbonInstanceParam;
typedef ::Effekseer::Vector3D efkVector3D;

class RibbonRenderer
	: public ::EffekseerRenderer::RibbonRendererBase
{
private:
	RendererImplemented*	m_renderer;
	
	RibbonRenderer(RendererImplemented* renderer);

public:

	virtual ~RibbonRenderer();

	static RibbonRenderer* Create( RendererImplemented* renderer );

public:
	void BeginRendering( const efkRibbonNodeParam& parameter, int32_t count, void* userData );

	void Rendering( const efkRibbonNodeParam& parameter, const efkRibbonInstanceParam& instanceParameter, void* userData );

	void EndRendering( const efkRibbonNodeParam& parameter, void* userData );
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRENDERER_GL_STRIPE_RENDERER_H__