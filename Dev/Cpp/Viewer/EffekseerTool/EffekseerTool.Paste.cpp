
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <EffekseerRenderer/EffekseerRendererDX9.Renderer.h>
#include <EffekseerRenderer/EffekseerRendererDX9.VertexBuffer.h>
#include <EffekseerRenderer/EffekseerRendererDX9.IndexBuffer.h>
#include <EffekseerRenderer/EffekseerRendererDX9.RenderState.h>

#include "EffekseerTool.Paste.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace Shader_
{
static
#include "EffekseerTool.Paste.Shader.h"
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Paste::Paste( EffekseerRendererDX9::RendererImplemented* renderer, ID3DXEffect* shader )
	: DeviceObject( renderer )
	, m_renderer	( renderer )
	, m_shader		( shader )
	, m_vertexDeclaration	( NULL )
{
	// 座標(3) 色(4)
	D3DVERTEXELEMENT9 decl[] =
	{
		{0,	0,	D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_POSITION,	0},
		{0,	12,	D3DDECLTYPE_FLOAT2,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TEXCOORD,	0},
		D3DDECL_END()
	};

	renderer->GetDevice()->CreateVertexDeclaration( decl, &m_vertexDeclaration );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Paste::~Paste()
{
	ES_SAFE_RELEASE( m_shader );
	ES_SAFE_RELEASE( m_vertexDeclaration );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Paste* Paste::Create( EffekseerRendererDX9::RendererImplemented* renderer )
{
	assert( renderer != NULL );
	assert( renderer->GetDevice() != NULL );

	HRESULT hr;
	ID3DXBuffer* buf = NULL;

	ID3DXEffect* shader = NULL;

	hr = D3DXCreateEffect(
		renderer->GetDevice(),
		Shader_::g_effect,
		sizeof(Shader_::g_effect),
		NULL,
		NULL,
		0,
		NULL,
		&shader,
		&buf);

	if( FAILED(hr) )
	{
		printf( "Grid Error\n");
		if( buf != NULL )
		{
			printf( (char*)buf->GetBufferPointer() );
			printf("\n");
			buf->Release();
		}
		else
		{
			printf( "Unknown Error\n" );
		}

		return NULL;
	}

	return new Paste( renderer, shader );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Paste::OnLostDevice()
{
	m_shader->OnLostDevice();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Paste::OnResetDevice()
{
	m_shader->OnResetDevice();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Paste::Rendering(IDirect3DTexture9* texture, int32_t width, int32_t height)
{
	if (texture == nullptr) return;

	m_renderer->GetVertexBuffer()->Lock();

	{
		auto offset_x = 0.5f / (float)width;
		auto offset_y = 0.5f / (float) height;

		Vertex* verteies = (Vertex*)m_renderer->GetVertexBuffer()->GetBufferDirect( sizeof(Vertex) * 4 );
		verteies[0].Pos.X = -1.0f;
		verteies[0].Pos.Y = -1.0f;
		verteies[0].Pos.Z = 0.5f;
		verteies[0].UV.X = 0.0f + offset_x;
		verteies[0].UV.Y = 1.0f + offset_y;

		verteies[1].Pos.X = -1.0f;
		verteies[1].Pos.Y = 1.0f;
		verteies[1].Pos.Z = 0.5f;
		verteies[1].UV.X = 0.0f + offset_x;
		verteies[1].UV.Y = 0.0f + offset_y;

		verteies[2].Pos.X = 1.0f;
		verteies[2].Pos.Y = -1.0f;
		verteies[2].Pos.Z = 0.5f;
		verteies[2].UV.X = 1.0f + offset_x;
		verteies[2].UV.Y = 1.0f + offset_y;

		verteies[3].Pos.X = 1.0f;
		verteies[3].Pos.Y = 1.0f;
		verteies[3].Pos.Z = 0.5f;
		verteies[3].UV.X = 1.0f + offset_x;
		verteies[3].UV.Y = 0.0f + offset_y;
	}

	m_renderer->GetVertexBuffer()->Unlock();

	auto& state = m_renderer->GetRenderState()->Push();
	state.AlphaBlend = ::Effekseer::AlphaBlendType::Blend;
	state.DepthWrite = false;
	state.DepthTest = true;
	state.CullingType = Effekseer::CullingType::Double;
	
	
	uint32_t pass;

	ID3DXEffect* shader = NULL;
	shader = m_shader;

	shader->SetTechnique( "Texhnique" );
	shader->Begin( &pass, 0 );

	assert( pass == 1 );

	shader->BeginPass(0);

	shader->SetTexture( "Tex0", texture );
	m_renderer->GetRenderState()->Update( true );
	
	shader->CommitChanges();

	m_renderer->GetDevice()->SetVertexDeclaration( m_vertexDeclaration );
	m_renderer->GetDevice()->SetStreamSource( 0, m_renderer->GetVertexBuffer()->GetInterface(), 0, sizeof(Vertex) );
	m_renderer->GetDevice()->SetIndices( m_renderer->GetIndexBuffer()->GetInterface() );
	m_renderer->GetDevice()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2 );

	shader->EndPass();
	shader->End();

	m_renderer->GetRenderState()->Pop();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
