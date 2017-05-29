
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <EffekseerRenderer/EffekseerRendererDX9.Renderer.h>
#include <EffekseerRenderer/EffekseerRendererDX9.VertexBuffer.h>
#include <EffekseerRenderer/EffekseerRendererDX9.IndexBuffer.h>
#include <EffekseerRenderer/EffekseerRendererDX9.Shader.h>
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
#include "EffekseerTool.Paste.Shader_VS.h"
#include "EffekseerTool.Paste.Shader_PS.h"
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Paste::Paste( EffekseerRendererDX9::RendererImplemented* renderer, EffekseerRendererDX9::Shader* shader)
	: DeviceObject( renderer )
	, m_renderer	( renderer )
	, m_shader		( shader )
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Paste::~Paste()
{
	ES_SAFE_DELETE( m_shader );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Paste* Paste::Create( EffekseerRendererDX9::RendererImplemented* renderer )
{
	assert( renderer != NULL );
	assert( renderer->GetDevice() != NULL );

	// 座標(3) 色(4)
	D3DVERTEXELEMENT9 decl[] =
	{
		{ 0,	0,	D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_POSITION,	0 },
		{ 0,	12,	D3DDECLTYPE_FLOAT2,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TEXCOORD,	0 },
		D3DDECL_END()
	};

	EffekseerRendererDX9::Shader* shader = EffekseerRendererDX9::Shader::Create(renderer, Shader_::g_vs20_VS, sizeof(Shader_::g_vs20_VS), Shader_::g_ps20_PS, sizeof(Shader_::g_ps20_PS), "Guide", decl);

	if (shader == NULL) return NULL;

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
	state.DepthTest = false;
	state.CullingType = Effekseer::CullingType::Double;
	state.TextureFilterTypes[0] = Effekseer::TextureFilterType::Nearest;
	state.TextureWrapTypes[0] = Effekseer::TextureWrapType::Clamp;

	m_renderer->BeginShader(m_shader);

	m_shader->SetConstantBuffer();

	m_renderer->GetRenderState()->Update(true);

	m_renderer->SetLayout(m_shader);
	m_renderer->GetDevice()->SetTexture(0, texture);
	m_renderer->GetDevice()->SetStreamSource(0, m_renderer->GetVertexBuffer()->GetInterface(), 0, sizeof(Vertex));
	m_renderer->GetDevice()->SetIndices(m_renderer->GetIndexBuffer()->GetInterface());
	m_renderer->GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

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
