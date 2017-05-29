
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <EffekseerRenderer/EffekseerRendererDX9.Renderer.h>
#include <EffekseerRenderer/EffekseerRendererDX9.VertexBuffer.h>
#include <EffekseerRenderer/EffekseerRendererDX9.IndexBuffer.h>
#include <EffekseerRenderer/EffekseerRendererDX9.Shader.h>
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
#include "EffekseerTool.Grid.Shader_VS.h"
#include "EffekseerTool.Grid.Shader_PS.h"
	}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Culling::Culling( EffekseerRendererDX9::RendererImplemented* renderer, EffekseerRendererDX9::Shader* shader)
	: DeviceObject( renderer )
	, m_renderer	( renderer )
	, m_shader		( shader )
	, m_lineCount		( 0 )
	, IsShown		(false)
	, Radius		(0.0f)
	, X				(0.0f)
	, Y				(0.0f)
	, Z				(0.0f)
{
	m_shader->SetVertexRegisterCount(4);
	m_shader->SetVertexConstantBufferSize(sizeof(float) * 16);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Culling::~Culling()
{
	ES_SAFE_DELETE(m_shader);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Culling* Culling::Create( EffekseerRendererDX9::RendererImplemented* renderer )
{
	assert(renderer != NULL);
	assert(renderer->GetDevice() != NULL);

	// 座標(3) 色(4)
	D3DVERTEXELEMENT9 decl[] =
	{
		{ 0,	0,	D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_POSITION,	0 },
		{ 0,	12,	D3DDECLTYPE_FLOAT4,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_NORMAL,	0 },
		D3DDECL_END()
	};

	auto shader = EffekseerRendererDX9::Shader::Create(renderer, Shader_::g_vs20_VS, sizeof(Shader_::g_vs20_VS), Shader_::g_ps20_PS, sizeof(Shader_::g_ps20_PS), "Grid", decl);

	if (shader == NULL) return NULL;

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

		for(int32_t r = 0; r < 9; r++)
		{
			float a0 = 3.1415f * 2.0f / 9.0f * r;
			float a1 = 3.1415f * 2.0f / 9.0f * (r + 1.0f);

			DrawLine( 
				::Effekseer::Vector3D( X + sin(a0) * radius, Y + ylen, Z + cos(a0) * radius ),
				::Effekseer::Vector3D( X + sin(a1) * radius, Y + ylen, Z + cos(a1) * radius ),
				::Effekseer::Color( 255, 255, 255, 255 ) );
		}
	}

	m_renderer->GetVertexBuffer()->Unlock();

	if( m_lineCount == 0 ) return;


	auto& state = m_renderer->GetRenderState()->Push();
	state.AlphaBlend = ::Effekseer::AlphaBlendType::Blend;
	state.DepthWrite = true;
	state.DepthTest = true;
	state.CullingType = Effekseer::CullingType::Double;

	m_renderer->BeginShader(m_shader);

	auto mat = m_renderer->GetCameraProjectionMatrix();
	mat = mat.Transpose();
	memcpy(m_shader->GetVertexConstantBuffer(), &(mat), sizeof(float) * 16);

	m_shader->SetConstantBuffer();

	m_renderer->GetRenderState()->Update(false);

	m_renderer->SetLayout(m_shader);
	m_renderer->GetDevice()->SetStreamSource(0, m_renderer->GetVertexBuffer()->GetInterface(), 0, sizeof(Vertex));
	m_renderer->GetDevice()->SetIndices(NULL);
	m_renderer->GetDevice()->DrawPrimitive(D3DPT_LINELIST, 0, m_lineCount);

	m_renderer->EndShader(m_shader);

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
