
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererDX9.RendererImplemented.h"
#include "EffekseerRendererDX9.RenderState.h"

#include "EffekseerRendererDX9.VertexBuffer.h"
#include "EffekseerRendererDX9.IndexBuffer.h"
#include "EffekseerRendererDX9.SpriteRenderer.h"
#include "EffekseerRendererDX9.Shader.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererDX9
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SpriteRenderer::SpriteRenderer( RendererImplemented* renderer )
	: m_renderer	( renderer )
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
SpriteRenderer* SpriteRenderer::Create( RendererImplemented* renderer )
{
	assert( renderer != NULL );
	assert( renderer->GetDevice() != NULL );

	return new SpriteRenderer( renderer );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SpriteRenderer::BeginRendering( const efkSpriteNodeParam& parameter, int32_t count, void* userData )
{
	BeginRendering_<RendererImplemented, Vertex>(m_renderer, count, parameter);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SpriteRenderer::Rendering( const efkSpriteNodeParam& parameter, const efkSpriteInstanceParam& instanceParameter, void* userData )
{
	if( m_spriteCount == m_renderer->GetSquareMaxCount() ) return;
	Rendering_<Vertex>( parameter, instanceParameter, userData, m_renderer->GetCameraMatrix() );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SpriteRenderer::EndRendering( const efkSpriteNodeParam& parameter, void* userData )
{
	if( m_ringBufferData == NULL ) return;

	m_renderer->GetVertexBuffer()->Unlock();

	if (m_spriteCount == 0) return;

	EndRendering_<RendererImplemented, Shader, IDirect3DTexture9*, Vertex>(m_renderer, parameter);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
