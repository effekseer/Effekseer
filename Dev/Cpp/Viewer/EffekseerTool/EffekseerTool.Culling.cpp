
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <EffekseerRenderer/EffekseerRendererDX9.Renderer.h>
#include <EffekseerRenderer/EffekseerRendererDX9.VertexBuffer.h>
#include <EffekseerRenderer/EffekseerRendererDX9.IndexBuffer.h>
#include <EffekseerRenderer/EffekseerRendererDX9.RenderState.h>
#include "EffekseerTool.Culling.h"

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
#include "EffekseerTool.Grid.Shader.h"
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Culling::Culling( EffekseerRendererDX9::RendererImplemented* renderer, ID3DXEffect* shader )
	: DeviceObject( renderer )
	, m_renderer	( renderer )
	, m_shader		( shader )
	, m_vertexDeclaration	( NULL )
	, m_lineCount		( 0 )
	, IsShown		(false)
	, Radius		(0.0f)
	, X				(0.0f)
	, Y				(0.0f)
	, Z				(0.0f)
{
	// À•W(3) F(4)
	D3DVERTEXELEMENT9 decl[] =
	{
		{0,	0,	D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_POSITION,	0},
		{0,	12,	D3DDECLTYPE_FLOAT4,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_NORMAL,	0},
		D3DDECL_END()
	};

	renderer->GetDevice()->CreateVertexDeclaration( decl, &m_vertexDeclaration );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Culling::~Culling()
{
	ES_SAFE_RELEASE( m_shader );
	ES_SAFE_RELEASE( m_vertexDeclaration );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Culling* Culling::Create( EffekseerRendererDX9::RendererImplemented* renderer )
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

	return new Culling( renderer, shader );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Culling::OnLostDevice()
{
	m_shader->OnLostDevice();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Culling::OnResetDevice()
{
	m_shader->OnResetDevice();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Culling::Rendering(bool isRightHand )
{
	if(!IsShown) return;

	m_lineCount = 0;

	m_renderer->GetVertexBuffer()->Lock();

	for(int32_t y = -3; y <= 3; y++)
	{
		float ylen = Radius * ((float)y / 4.0f);
		float radius = sqrt(Radius * Radius - ylen * ylen);

		for(int32_t r = 0; r < 8; r++)
		{
			float a0 = 3.1415f * 2.0f / 9.0f * r;
			float a1 = 3.1415f * 2.0f / 9.0f * (r * 1.0f);

			DrawLine( 
				::Effekseer::Vector3D( X + sin(a0) * Radius, Y + ylen, Z + cos(a1) * Radius ),
				::Effekseer::Vector3D( X + sin(a0) * Radius, Y + ylen, Z + cos(a1) * Radius ),
				::Effekseer::Color( 255, 255, 255, 255 ) );
		}
	}

	m_renderer->GetVertexBuffer()->Unlock();

	if( m_lineCount == 0 ) return;

	auto& state = m_renderer->GetRenderState()->Push();
	state.AlphaBlend = ::Effekseer::ALPHA_BLEND_BLEND;
	state.DepthWrite = true;
	state.DepthTest = true;
	state.CullingType = Effekseer::CULLING_DOUBLE;
	
	
	uint32_t pass;

	ID3DXEffect* shader = NULL;
	shader = m_shader;

	shader->SetTechnique( "Texhnique" );
	shader->Begin( &pass, 0 );

	assert( pass == 1 );

	shader->BeginPass(0);

	shader->SetMatrix( "mCameraProj", (const D3DXMATRIX *)&m_renderer->GetCameraProjectionMatrix() );

	m_renderer->GetRenderState()->Update( false );
	shader->CommitChanges();

	m_renderer->GetDevice()->SetVertexDeclaration( m_vertexDeclaration );
	m_renderer->GetDevice()->SetStreamSource( 0, m_renderer->GetVertexBuffer()->GetInterface(), 0, sizeof(Vertex) );
	m_renderer->GetDevice()->SetIndices( NULL );
	m_renderer->GetDevice()->DrawPrimitive( D3DPT_LINELIST, 0, m_lineCount );

	shader->EndPass();
	shader->End();

	m_renderer->GetRenderState()->Pop();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Culling::DrawLine( const ::Effekseer::Vector3D& pos1, const ::Effekseer::Vector3D& pos2, const ::Effekseer::Color& col )
{
	Vertex* verteies = (Vertex*)m_renderer->GetVertexBuffer()->GetBufferDirect( sizeof(Vertex) * 2 );

	verteies[0].Pos = pos1;
	verteies[1].Pos = pos2;
	
	for( int i = 0; i < 2; i++ )
	{
		verteies[i].Col[0] = (float)col.R / 255.0f;
		verteies[i].Col[1] = (float)col.G / 255.0f;
		verteies[i].Col[2] = (float)col.B / 255.0f;
		verteies[i].Col[3] = (float)col.A / 255.0f;
	}

	m_lineCount++;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
