
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <EffekseerRenderer/EffekseerRendererDX9.Renderer.h>
#include <EffekseerRenderer/EffekseerRendererDX9.VertexBuffer.h>
#include <EffekseerRenderer/EffekseerRendererDX9.IndexBuffer.h>
#include <EffekseerRenderer/EffekseerRendererDX9.Shader.h>
#include <EffekseerRenderer/EffekseerRendererDX9.RenderState.h>
#include "EffekseerTool.Guide.h"

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
#include "EffekseerTool.Guide.Shader_VS.h"
#include "EffekseerTool.Guide.Shader_PS.h"
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Guide::Guide( EffekseerRendererDX9::RendererImplemented* renderer, EffekseerRendererDX9::Shader* shader )
	: m_renderer	( renderer )
	, m_shader		( shader )
{
	m_shader->SetVertexRegisterCount(1);
	m_shader->SetVertexConstantBufferSize(sizeof(float)*4);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Guide::~Guide()
{
	ES_SAFE_DELETE( m_shader );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Guide* Guide::Create( EffekseerRendererDX9::RendererImplemented* renderer )
{
	assert( renderer != NULL );
	assert( renderer->GetDevice() != NULL );

	// 座標(2)
	D3DVERTEXELEMENT9 decl[] =
	{
		{0,	0,	D3DDECLTYPE_FLOAT2,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_POSITION,	0},
		D3DDECL_END()
	};

	EffekseerRendererDX9::Shader* shader = EffekseerRendererDX9::Shader::Create( renderer, Shader_::g_vs20_VS, sizeof(Shader_::g_vs20_VS), Shader_::g_ps20_PS, sizeof(Shader_::g_ps20_PS), "Guide", decl );
	
	if( shader == NULL ) return NULL;

	return new Guide( renderer, shader );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Guide::OnLostDevice()
{
	m_shader->OnLostDevice();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Guide::OnResetDevice()
{
	m_shader->OnResetDevice();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Guide::Rendering( int32_t width, int32_t height, int32_t guide_width, int32_t guide_height )
{
	m_renderer->GetVertexBuffer()->Lock();

	float ul_x = 0;
	float ul_y = 0;
	float ul_gx = (width - guide_width) / 2.0f;
	float ul_gy = (height - guide_height) / 2.0f;

	float ur_x = ul_x + width;
	float ur_y = ul_y;
	float ur_gx = ul_gx + guide_width;
	float ur_gy = ul_gy;

	float dl_x = ul_x;
	float dl_y = ul_y + height;
	float dl_gx = ul_gx;
	float dl_gy = ul_gy + guide_height;

	float dr_x = ur_x;
	float dr_y = dl_y;
	float dr_gx = ur_gx;
	float dr_gy = dl_gy;

	{
		Vertex* verteies = (Vertex*)m_renderer->GetVertexBuffer()->GetBufferDirect( sizeof(Vertex) * 4 );
		verteies[0].x = ul_gx;
		verteies[0].y = ul_gy;
		verteies[1].x = ul_x;
		verteies[1].y = ul_y;
		verteies[3].x = ur_x;
		verteies[3].y = ur_y;
		verteies[2].x = ur_gx;
		verteies[2].y = ur_gy;
	}

	{
		Vertex* verteies = (Vertex*)m_renderer->GetVertexBuffer()->GetBufferDirect( sizeof(Vertex) * 4 );
		verteies[0].x = ul_x;
		verteies[0].y = ul_y;
		verteies[1].x = ul_gx;
		verteies[1].y = ul_gy;
		verteies[3].x = dl_gx;
		verteies[3].y = dl_gy;
		verteies[2].x = dl_x;
		verteies[2].y = dl_y;
	}

	{
		Vertex* verteies = (Vertex*)m_renderer->GetVertexBuffer()->GetBufferDirect( sizeof(Vertex) * 4 );
		verteies[0].x = dl_x;
		verteies[0].y = dl_y;
		verteies[1].x = dl_gx;
		verteies[1].y = dl_gy;
		verteies[3].x = dr_gx;
		verteies[3].y = dr_gy;
		verteies[2].x = dr_x;
		verteies[2].y = dr_y;
	}

	{
		Vertex* verteies = (Vertex*)m_renderer->GetVertexBuffer()->GetBufferDirect( sizeof(Vertex) * 4 );
		verteies[0].x = ur_x;
		verteies[0].y = ur_y;
		verteies[1].x = dr_x;
		verteies[1].y = dr_y;
		verteies[3].x = dr_gx;
		verteies[3].y = dr_gy;
		verteies[2].x = ur_gx;
		verteies[2].y = ur_gy;
	}

	m_renderer->GetVertexBuffer()->Unlock();

	auto& state = m_renderer->GetRenderState()->Push();
	state.AlphaBlend = ::Effekseer::AlphaBlendType::Blend;
	state.DepthWrite = false;
	state.DepthTest = false;
	state.CullingType = Effekseer::CullingType::Double;

	m_renderer->BeginShader(m_shader);

	((float*)m_shader->GetVertexConstantBuffer())[0] = (float)width;
	((float*)m_shader->GetVertexConstantBuffer())[1] = (float)height;

	m_shader->SetConstantBuffer();

	m_renderer->GetRenderState()->Update( false );
	
	m_renderer->SetLayout( m_shader );
	m_renderer->GetDevice()->SetStreamSource( 0, m_renderer->GetVertexBuffer()->GetInterface(), 0, sizeof(Vertex) );
	m_renderer->GetDevice()->SetIndices( m_renderer->GetIndexBuffer()->GetInterface() );
	m_renderer->GetDevice()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 4 * 4, 0, 4 * 2 );

	m_renderer->EndShader(m_shader);

	m_renderer->GetRenderState()->Pop();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
