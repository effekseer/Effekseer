
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererDX11.RendererImplemented.h"
#include "EffekseerRendererDX11.RenderState.h"

#include "EffekseerRendererDX11.VertexBuffer.h"
#include "EffekseerRendererDX11.IndexBuffer.h"
#include "EffekseerRendererDX11.TrackRenderer.h"
#include "EffekseerRendererDX11.Shader.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererDX11
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
TrackRenderer::TrackRenderer( RendererImplemented* renderer)
	: m_renderer	( renderer )
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
TrackRenderer::~TrackRenderer()
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
TrackRenderer* TrackRenderer::Create( RendererImplemented* renderer )
{
	assert( renderer != NULL );
	assert( renderer->GetDevice() != NULL );

	return new TrackRenderer( renderer );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void TrackRenderer::BeginRendering( const efkTrackNodeParam& parameter, int32_t count, void* userData )
{
	BeginRendering_<Vertex>( m_renderer, parameter, count, userData );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void TrackRenderer::Rendering( const efkTrackNodeParam& parameter, const efkTrackInstanceParam& instanceParameter, void* userData )
{
	Rendering_<Vertex, VertexDistortion>(parameter, instanceParameter, userData, m_renderer->GetCameraMatrix());
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void TrackRenderer::EndRendering( const efkTrackNodeParam& parameter, void* userData )
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
