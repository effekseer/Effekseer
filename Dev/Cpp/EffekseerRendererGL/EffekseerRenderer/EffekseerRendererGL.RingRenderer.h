
#ifndef	__EFFEKSEERRENDERER_GL_RING_RENDERER_H__
#define	__EFFEKSEERRENDERER_GL_RING_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <memory>
#include "EffekseerRendererGL.RendererImplemented.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.RingRendererBase.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererGL
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
typedef ::Effekseer::RingRenderer::NodeParameter efkRingNodeParam;
typedef ::Effekseer::RingRenderer::InstanceParameter efkRingInstanceParam;
typedef ::Effekseer::Vector3D efkVector3D;

class RingRenderer
	: public ::EffekseerRenderer::RingRendererBase
{
private:
	RendererImplemented*	m_renderer;

	RingRenderer(RendererImplemented* renderer);

public:

	virtual ~RingRenderer();

	static RingRenderer* Create(RendererImplemented* renderer);

public:
	void BeginRendering(const efkRingNodeParam& parameter, int32_t count, void* userData);

	void Rendering(const efkRingNodeParam& parameter, const efkRingInstanceParam& instanceParameter, void* userData);

	void EndRendering(const efkRingNodeParam& parameter, void* userData);
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRENDERER_GL_RING_RENDERER_H__