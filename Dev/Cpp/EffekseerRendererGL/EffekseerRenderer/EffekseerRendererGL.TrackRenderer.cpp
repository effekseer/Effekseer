
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererGL.RendererImplemented.h"
#include "EffekseerRendererGL.RenderState.h"

#include "EffekseerRendererGL.VertexBuffer.h"
#include "EffekseerRendererGL.IndexBuffer.h"
#include "EffekseerRendererGL.TrackRenderer.h"
#include "EffekseerRendererGL.Shader.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererGL
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
TrackRenderer::TrackRenderer(RendererImplemented* renderer)
	: m_renderer(renderer)
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
TrackRenderer* TrackRenderer::Create(RendererImplemented* renderer)
{
	assert(renderer != NULL);

	return new TrackRenderer(renderer);
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
	Rendering_<Vertex>( parameter, instanceParameter, userData, m_renderer->GetCameraMatrix() );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void TrackRenderer::EndRendering(const efkTrackNodeParam& parameter, void* userData)
{
	if (m_ringBufferData == NULL) return;

	m_renderer->GetVertexBuffer()->Unlock();

	if (m_ribbonCount <= 1) return;

	EndRendering_<RendererImplemented, GLuint, Vertex>(m_renderer, parameter);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
