
#include "efk.LineRendererDX9.h"

namespace efk
{
	namespace Standard_VS
	{
		static
#include <EffekseerRendererDX9/EffekseerRenderer/Shader/EffekseerRenderer.Tool_VS.h>
	}

	namespace Standard_PS
	{
		static
#include <EffekseerRendererDX9/EffekseerRenderer/Shader/EffekseerRenderer.Tool_PS.h>
	}

	namespace StandardNoTexture_PS
	{
		static
#include <EffekseerRendererDX9/EffekseerRenderer/Shader/EffekseerRenderer.ToolNoTexture_PS.h>
	}

	LineRendererDX9::LineRendererDX9(EffekseerRenderer::Renderer* renderer)
		: LineRenderer(renderer)
	{
		this->renderer = (EffekseerRendererDX9::RendererImplemented*)renderer;


		// Position(3) Color(1) UV(2)
		D3DVERTEXELEMENT9 decl[] =
		{
			{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
			{ 0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
			D3DDECL_END()
		};

		shader = EffekseerRendererDX9::Shader::Create(
			this->renderer,
			Standard_VS::g_vs20_VS,
			sizeof(Standard_VS::g_vs20_VS),
			StandardNoTexture_PS::g_ps20_PS,
			sizeof(StandardNoTexture_PS::g_ps20_PS),
			"StandardRenderer No Texture",
			decl);

		shader->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44) * 2);
		((EffekseerRendererDX9::Shader*)shader)->SetVertexRegisterCount(8);
	}

	LineRendererDX9::~LineRendererDX9()
	{
		auto shader_ = (EffekseerRendererDX9::Shader*)shader;
		ES_SAFE_DELETE(shader_);
		shader = nullptr;
	}

	void LineRendererDX9::DrawLine(const Effekseer::Vector3D& p1, const Effekseer::Vector3D& p2, const Effekseer::Color& c)
	{
		EffekseerRendererDX9::Vertex v0;
		v0.Pos = p1;
		v0.Col = D3DCOLOR_ARGB(c.A, c.R, c.G, c.B);

		EffekseerRendererDX9::Vertex v1;
		v1.Pos = p2;
		v1.Col = D3DCOLOR_ARGB(c.A, c.R, c.G, c.B);

		vertexies.push_back(v0);
		vertexies.push_back(v1);
	}

	void LineRendererDX9::Render()
	{
		if (vertexies.size() == 0) return;

		renderer->GetVertexBuffer()->Lock();

		auto vs = (EffekseerRendererDX9::Vertex*)renderer->GetVertexBuffer()->GetBufferDirect(sizeof(EffekseerRendererDX9::Vertex) * vertexies.size());

		memcpy(vs, vertexies.data(), sizeof(EffekseerRendererDX9::Vertex) * vertexies.size());

		renderer->GetVertexBuffer()->Unlock();

		auto& state = renderer->GetRenderState()->Push();
		state.AlphaBlend = ::Effekseer::AlphaBlendType::Blend;
		state.DepthWrite = true;
		state.DepthTest = true;
		state.CullingType = Effekseer::CullingType::Double;

		renderer->SetRenderMode(Effekseer::RenderMode::Normal);
		renderer->BeginShader((EffekseerRendererDX9::Shader*)shader);

		((Effekseer::Matrix44*)(shader->GetVertexConstantBuffer()))[0] = renderer->GetCameraMatrix();
		((Effekseer::Matrix44*)(shader->GetVertexConstantBuffer()))[1] = renderer->GetProjectionMatrix();

		shader->SetConstantBuffer();

		renderer->GetRenderState()->Update(false);

		renderer->SetLayout((EffekseerRendererDX9::Shader*)shader);
		renderer->GetDevice()->SetStreamSource(0, renderer->GetVertexBuffer()->GetInterface(), 0, sizeof(EffekseerRendererDX9::Vertex));
		renderer->GetDevice()->SetIndices(NULL);
		renderer->GetDevice()->DrawPrimitive(D3DPT_LINELIST, 0, vertexies.size() / 2);

		renderer->EndShader((EffekseerRendererDX9::Shader*)shader);

		renderer->GetRenderState()->Pop();
	}


	void LineRendererDX9::ClearCache()
	{
		vertexies.clear();
	}

	void LineRendererDX9::OnLostDevice()
	{
		auto shader_ = (EffekseerRendererDX9::Shader*)shader;
		shader_->OnLostDevice();
	}

	void LineRendererDX9::OnResetDevice()
	{
		auto shader_ = (EffekseerRendererDX9::Shader*)shader;
		shader_->OnResetDevice();
	}
}
