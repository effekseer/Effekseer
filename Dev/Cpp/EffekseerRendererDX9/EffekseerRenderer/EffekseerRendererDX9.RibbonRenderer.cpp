
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererDX9.RendererImplemented.h"
#include "EffekseerRendererDX9.RenderState.h"

#include "EffekseerRendererDX9.VertexBuffer.h"
#include "EffekseerRendererDX9.IndexBuffer.h"
#include "EffekseerRendererDX9.RibbonRenderer.h"
#include "EffekseerRendererDX9.Shader.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererDX9
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RibbonRenderer::RibbonRenderer( RendererImplemented* renderer )
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

	EndRendering_<RendererImplemented, IDirect3DTexture9*, Vertex>(m_renderer, parameter);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
