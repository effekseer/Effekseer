
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
	Shader*		m_shader;
	Shader*		m_shader_no_texture;
	
	std::unique_ptr<VertexArray>	m_vao;
	std::unique_ptr<VertexArray>	m_vao_no_texture;

	RingRenderer(RendererImplemented* renderer, Shader* shader, Shader* shader_no_texture);

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