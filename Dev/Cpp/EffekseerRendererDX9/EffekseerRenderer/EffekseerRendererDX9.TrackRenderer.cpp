
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererDX9.RendererImplemented.h"
#include "EffekseerRendererDX9.RenderState.h"

#include "EffekseerRendererDX9.VertexBuffer.h"
#include "EffekseerRendererDX9.IndexBuffer.h"
#include "EffekseerRendererDX9.TrackRenderer.h"
#include "EffekseerRendererDX9.Shader.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererDX9
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
TrackRenderer::TrackRenderer( RendererImplemented* renderer )
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
	BeginRendering_<Vertex>(m_renderer, parameter, count, userData);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void TrackRenderer::Rendering( const efkTrackNodeParam& parameter, const efkTrackInstanceParam& instanceParameter, void* userData )
{
	Rendering_<Vertex>( parameter, instanceParameter, userData, m_renderer->GetCameraMatrix() );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void TrackRenderer::EndRendering( const efkTrackNodeParam& parameter, void* userData )
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
