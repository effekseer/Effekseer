
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererGL.RendererImplemented.h"
#include "EffekseerRendererGL.RenderState.h"

#include "EffekseerRendererGL.VertexBuffer.h"
#include "EffekseerRendererGL.IndexBuffer.h"
#include "EffekseerRendererGL.RibbonRenderer.h"
#include "EffekseerRendererGL.Shader.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererGL
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RibbonRenderer::RibbonRenderer(RendererImplemented* renderer)
	: m_renderer	( renderer )
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RibbonRenderer::~RibbonRenderer()
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RibbonRenderer* RibbonRenderer::Create(RendererImplemented* renderer)
{
	assert(renderer != NULL);

	return new RibbonRenderer(renderer);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RibbonRenderer::BeginRendering( const efkRibbonNodeParam& parameter, int32_t count, void* userData )
{
	BeginRendering_<RendererImplemented, Vertex>(m_renderer, count, parameter);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RibbonRenderer::Rendering( const efkRibbonNodeParam& parameter, const efkRibbonInstanceParam& instanceParameter, void* userData )
{
	Rendering_<Vertex>( parameter, instanceParameter, userData, m_renderer->GetCameraMatrix() );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RibbonRenderer::EndRendering( const efkRibbonNodeParam& parameter, void* userData )
{
	if( m_ringBufferData == NULL ) return;

	m_renderer->GetVertexBuffer()->Unlock();

	if( m_ribbonCount <= 1 ) return;
	
	EndRendering_<RendererImplemented, GLuint, Vertex>(m_renderer, parameter);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
