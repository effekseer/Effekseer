
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererDX11.RendererImplemented.h"
#include "EffekseerRendererDX11.RenderState.h"

#include "EffekseerRendererDX11.VertexBuffer.h"
#include "EffekseerRendererDX11.IndexBuffer.h"
#include "EffekseerRendererDX11.RibbonRenderer.h"
#include "EffekseerRendererDX11.Shader.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererDX11
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RibbonRenderer::RibbonRenderer( RendererImplemented* renderer)
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
RibbonRenderer* RibbonRenderer::Create( RendererImplemented* renderer )
{
	assert( renderer != NULL );
	assert( renderer->GetDevice() != NULL );

	return new RibbonRenderer( renderer );
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
	Rendering_<Vertex, VertexDistortion>(parameter, instanceParameter, userData, m_renderer->GetCameraMatrix());
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RibbonRenderer::EndRendering( const efkRibbonNodeParam& parameter, void* userData )
{
	if( m_ringBufferData == NULL ) return;

	if( m_ribbonCount <= 1 ) return;

	EndRendering_<RendererImplemented, ID3D11ShaderResourceView*, Vertex>(m_renderer, parameter);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
