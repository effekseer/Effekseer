
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <math.h>
#include "EffekseerRendererGL.RendererImplemented.h"
#include "EffekseerRendererGL.RenderState.h"

#include "EffekseerRendererGL.VertexBuffer.h"
#include "EffekseerRendererGL.IndexBuffer.h"
#include "EffekseerRendererGL.VertexArray.h"
#include "EffekseerRendererGL.RingRenderer.h"
#include "EffekseerRendererGL.Shader.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererGL
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RingRenderer::RingRenderer(RendererImplemented* renderer)
	: m_renderer(renderer)

{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RingRenderer::~RingRenderer()
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RingRenderer* RingRenderer::Create(RendererImplemented* renderer)
{
	assert(renderer != NULL);

	return new RingRenderer(renderer);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RingRenderer::BeginRendering( const efkRingNodeParam& parameter, int32_t count, void* userData )
{
	BeginRendering_<RendererImplemented, Vertex>(m_renderer, count, parameter);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RingRenderer::Rendering( const efkRingNodeParam& parameter, const efkRingInstanceParam& instanceParameter, void* userData )
{
	if( m_spriteCount == m_renderer->GetSquareMaxCount() ) return;
	Rendering_<Vertex>( parameter, instanceParameter, userData, m_renderer->GetCameraMatrix() );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RingRenderer::EndRendering( const efkRingNodeParam& parameter, void* userData )
{
	if( m_spriteCount == 0 ) return;
	
	EndRendering_<RendererImplemented, Shader, GLuint, Vertex>(m_renderer, parameter);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
