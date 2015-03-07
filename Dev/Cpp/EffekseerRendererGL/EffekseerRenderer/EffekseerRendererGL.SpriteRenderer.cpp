
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <math.h>
#include "EffekseerRendererGL.RendererImplemented.h"
#include "EffekseerRendererGL.RenderState.h"

#include "EffekseerRendererGL.VertexBuffer.h"
#include "EffekseerRendererGL.IndexBuffer.h"
#include "EffekseerRendererGL.VertexArray.h"
#include "EffekseerRendererGL.SpriteRenderer.h"
#include "EffekseerRendererGL.Shader.h"
#include "EffekseerRendererGL.GLExtension.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererGL
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SpriteRenderer::SpriteRenderer(RendererImplemented* renderer)
	: m_renderer(renderer)
{

}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SpriteRenderer::~SpriteRenderer()
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SpriteRenderer* SpriteRenderer::Create(RendererImplemented* renderer)
{
	assert(renderer != NULL);
	return new SpriteRenderer(renderer);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SpriteRenderer::BeginRendering(const efkSpriteNodeParam& parameter, int32_t count, void* userData)
{
	BeginRendering_<RendererImplemented, Vertex>(m_renderer, count, parameter);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SpriteRenderer::Rendering(const efkSpriteNodeParam& parameter, const efkSpriteInstanceParam& instanceParameter, void* userData)
{
	if (m_spriteCount == m_renderer->GetSquareMaxCount()) return;
	Rendering_<Vertex>(parameter, instanceParameter, userData, m_renderer->GetCameraMatrix());
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SpriteRenderer::EndRendering( const efkSpriteNodeParam& parameter, void* userData )
{
	if( m_ringBufferData == NULL ) return;

	m_renderer->GetVertexBuffer()->Unlock();

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
