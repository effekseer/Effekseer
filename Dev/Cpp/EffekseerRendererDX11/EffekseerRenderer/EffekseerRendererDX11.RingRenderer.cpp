
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <math.h>
#include "EffekseerRendererDX11.RendererImplemented.h"
#include "EffekseerRendererDX11.RenderState.h"

#include "EffekseerRendererDX11.VertexBuffer.h"
#include "EffekseerRendererDX11.IndexBuffer.h"
#include "EffekseerRendererDX11.RingRenderer.h"
#include "EffekseerRendererDX11.Shader.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererDX11
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RingRenderer::RingRenderer( RendererImplemented* renderer )
	: m_renderer	( renderer )
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
RingRenderer* RingRenderer::Create( RendererImplemented* renderer )
{
	assert( renderer != NULL );
	assert( renderer->GetDevice() != NULL );

	return new RingRenderer( renderer );
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
	Rendering_<Vertex, VertexDistortion>(parameter, instanceParameter, userData, m_renderer->GetCameraMatrix());
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RingRenderer::EndRendering( const efkRingNodeParam& parameter, void* userData )
{
	if( m_ringBufferData == NULL ) return;

	if( m_spriteCount == 0 ) return;

	EndRendering_<RendererImplemented, Shader,ID3D11ShaderResourceView*, Vertex>(m_renderer, parameter);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
