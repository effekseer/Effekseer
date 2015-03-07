
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <math.h>
#include "EffekseerRendererDX9.RendererImplemented.h"
#include "EffekseerRendererDX9.RenderState.h"

#include "EffekseerRendererDX9.VertexBuffer.h"
#include "EffekseerRendererDX9.IndexBuffer.h"
#include "EffekseerRendererDX9.RingRenderer.h"
#include "EffekseerRendererDX9.Shader.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererDX9
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace Standard_VS
{
static
#include "Shader/EffekseerRenderer.Standard_VS.h"
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace Standard_PS
{
static
#include "Shader/EffekseerRenderer.Standard_PS.h"
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace StandardNoTexture_PS
{
static
#include "Shader/EffekseerRenderer.StandardNoTexture_PS.h"
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RingRenderer::RingRenderer( RendererImplemented* renderer, Shader* shader, Shader* shader_no_texture )
	: m_renderer	( renderer )
	, m_shader		( shader )
	, m_shader_no_texture		( shader_no_texture )
{
	shader->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44));
	shader->SetVertexRegisterCount(4);
	shader_no_texture->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44));
	shader_no_texture->SetVertexRegisterCount(4);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RingRenderer::~RingRenderer()
{
	ES_SAFE_DELETE( m_shader );
	ES_SAFE_DELETE( m_shader_no_texture );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RingRenderer* RingRenderer::Create( RendererImplemented* renderer )
{
	assert( renderer != NULL );
	assert( renderer->GetDevice() != NULL );

	// À•W(3) F(1) UV(2)
	D3DVERTEXELEMENT9 decl[] =
	{
		{0,	0,	D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_POSITION,	0},
		{0,	12,	D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_NORMAL,	0},
		{0,	16,	D3DDECLTYPE_FLOAT2,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TEXCOORD,	0},
		D3DDECL_END()
	};

	//ID3DXBuffer* buf = NULL;

	Shader* shader = Shader::Create( 
		renderer, 
		Standard_VS::g_vs20_VS, 
		sizeof(Standard_VS::g_vs20_VS), 
		Standard_PS::g_ps20_PS, 
		sizeof(Standard_PS::g_ps20_PS), 
		"StandardRenderer", decl );
	if( shader == NULL ) return NULL;

	Shader* shader_no_texture = Shader::Create( 
		renderer, 
		Standard_VS::g_vs20_VS, 
		sizeof(Standard_VS::g_vs20_VS),
		StandardNoTexture_PS::g_ps20_PS, 
		sizeof(StandardNoTexture_PS::g_ps20_PS), 
		"StandardRenderer No Texture", 
		decl );

	if( shader_no_texture == NULL )
	{
		ES_SAFE_DELETE( shader );
		return NULL;
	}

	return new RingRenderer( renderer, shader, shader_no_texture );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RingRenderer::BeginRendering( const efkRingNodeParam& parameter, int32_t count, void* userData )
{
	m_spriteCount = 0;
	
	int32_t vertexCount = parameter.VertexCount * 8;

	if( ! m_renderer->GetVertexBuffer()->RingBufferLock( count * sizeof(Vertex) * vertexCount, m_ringBufferOffset, (void*&)m_ringBufferData ) )
	{
		m_ringBufferOffset = 0;
		m_ringBufferData = NULL;
	}

	m_instanceCount = count;
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
	if( m_ringBufferData == NULL ) return;

	m_renderer->GetVertexBuffer()->Unlock();

	if( m_spriteCount == 0 ) return;

	EndRendering_<RendererImplemented, Shader, IDirect3DTexture9*, Vertex>(m_renderer, m_shader, m_shader_no_texture, parameter);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
