
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
TrackRenderer::TrackRenderer( RendererImplemented* renderer, Shader* shader, Shader* shader_no_texture )
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
TrackRenderer::~TrackRenderer()
{
	ES_SAFE_DELETE( m_shader );
	ES_SAFE_DELETE( m_shader_no_texture );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
TrackRenderer* TrackRenderer::Create( RendererImplemented* renderer )
{
	assert( renderer != NULL );
	assert( renderer->GetDevice() != NULL );

		// ç¿ïW(3) êF(1) UV(2)
	D3D11_INPUT_ELEMENT_DESC decl[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, sizeof(float) * 3, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float) * 4, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

	Shader* shader = Shader::Create( 
		renderer, 
		Standard_VS::g_VS, 
		sizeof(Standard_VS::g_VS),
		Standard_PS::g_PS, 
		sizeof(Standard_PS::g_PS),
		"StandardRenderer", decl, ARRAYSIZE(decl) );
	if( shader == NULL ) return NULL;

	Shader* shader_no_texture = Shader::Create( 
		renderer, 
		Standard_VS::g_VS, 
		sizeof(Standard_VS::g_VS),
		StandardNoTexture_PS::g_PS, 
		sizeof(StandardNoTexture_PS::g_PS),
		"StandardRenderer No Texture", 
		decl, ARRAYSIZE(decl) );

	return new TrackRenderer( renderer, shader, shader_no_texture );
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
void TrackRenderer::EndRendering( const efkTrackNodeParam& parameter, void* userData )
{
	if( m_ringBufferData == NULL ) return;

	m_renderer->GetVertexBuffer()->Unlock();

	if( m_ribbonCount <= 1 ) return;

	EndRendering_<RendererImplemented, Shader, ID3D11ShaderResourceView*, Vertex>(m_renderer, m_shader, m_shader_no_texture, parameter);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
