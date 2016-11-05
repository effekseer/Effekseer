
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <EffekseerRenderer/EffekseerRendererDX9.Renderer.h>
#include <EffekseerRenderer/EffekseerRendererDX9.VertexBuffer.h>
#include <EffekseerRenderer/EffekseerRendererDX9.IndexBuffer.h>
#include <EffekseerRenderer/EffekseerRendererDX9.RenderState.h>
#include "EffekseerTool.Grid.h"

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
Grid::Grid( EffekseerRendererDX9::RendererImplemented* renderer, ID3DXEffect* shader )
	: DeviceObject( renderer )
	, m_renderer	( renderer )
	, m_shader		( shader )
	, m_vertexDeclaration	( NULL )
	, m_lineCount		( 0 )
	, m_gridLength		( 2.0f )
	, IsShownXY	( false )
	, IsShownXZ	( true )
	, IsShownYZ	( false )
{
	// 座標(3) 色(4)
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
Grid::~Grid()
{
	ES_SAFE_RELEASE( m_shader );
	ES_SAFE_RELEASE( m_vertexDeclaration );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Grid* Grid::Create( EffekseerRendererDX9::RendererImplemented* renderer )
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

	return new Grid( renderer, shader );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Grid::OnLostDevice()
{
	m_shader->OnLostDevice();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Grid::OnResetDevice()
{
	m_shader->OnResetDevice();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Grid::Rendering( ::Effekseer::Color& gridColor, bool isRightHand )
{
	m_lineCount = 0;

	m_renderer->GetVertexBuffer()->Lock();

	if( IsShownXZ )
	{
		for( int i = -5; i <= 5; i++ )
		{
			DrawLine( 
				::Effekseer::Vector3D( i * m_gridLength, 0.0f, m_gridLength * 5.0f ),
				::Effekseer::Vector3D( i * m_gridLength, 0.0f, -m_gridLength * 5.0f ),
				gridColor );

			DrawLine( 
				::Effekseer::Vector3D( m_gridLength * 5.0f, 0.0f, i * m_gridLength ),
				::Effekseer::Vector3D( -m_gridLength * 5.0f, 0.0f, i * m_gridLength ),
				gridColor );
		}
	}

	if( IsShownXY )
	{
		for( int i = -5; i <= 5; i++ )
		{
			DrawLine( 
				::Effekseer::Vector3D( i * m_gridLength, m_gridLength * 5.0f, 0.0f ),
				::Effekseer::Vector3D( i * m_gridLength, -m_gridLength * 5.0f, 0.0f ),
				gridColor );

			DrawLine( 
				::Effekseer::Vector3D( m_gridLength * 5.0f, i * m_gridLength, 0.0f ),
				::Effekseer::Vector3D( -m_gridLength * 5.0f, i * m_gridLength, 0.0f ),
				gridColor );
		}
	}

	if( IsShownYZ )
	{
		for( int i = -5; i <= 5; i++ )
		{
			DrawLine( 
				::Effekseer::Vector3D( 0.0f, i * m_gridLength, m_gridLength * 5.0f ),
				::Effekseer::Vector3D( 0.0f, i * m_gridLength, -m_gridLength * 5.0f ),
				gridColor );

			DrawLine( 
				::Effekseer::Vector3D( 0.0f, m_gridLength * 5.0f, i * m_gridLength ),
				::Effekseer::Vector3D( 0.0f, -m_gridLength * 5.0f, i * m_gridLength ),
				gridColor );
		}
	}


	DrawLine( 
			::Effekseer::Vector3D( 0.0f, 0.0f, 0.0f ),
			::Effekseer::Vector3D( m_gridLength, 0.0f, 0.0f ),
			::Effekseer::Color( 255, 0, 0, 255 ) );

	DrawLine( 
			::Effekseer::Vector3D( 0.0f, 0.0f, 0.0f ),
			::Effekseer::Vector3D( 0.0f, m_gridLength, 0.0f ),
			::Effekseer::Color( 0, 255, 0, 255 ) );

	DrawLine( 
		::Effekseer::Vector3D( 0.0f, 0.0f, 0.0f ),
		::Effekseer::Vector3D( 0.0f, 0.0f, m_gridLength ),
		::Effekseer::Color( 0, 0, 255, 255 ) );

	m_renderer->GetVertexBuffer()->Unlock();

	if( m_lineCount == 0 ) return;

	auto& state = m_renderer->GetRenderState()->Push();
	state.AlphaBlend = ::Effekseer::AlphaBlendType::Blend;
	state.DepthWrite = true;
	state.DepthTest = true;
	state.CullingType = Effekseer::CullingType::Double;
	
	
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
void Grid::DrawLine( const ::Effekseer::Vector3D& pos1, const ::Effekseer::Vector3D& pos2, const ::Effekseer::Color& col )
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
