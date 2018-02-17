
#include "efk.LineRendererDX9.h"

namespace efk
{
	namespace Shader_
	{
		static
#include "../../../EffekseerTool/EffekseerTool.Grid.Shader_VS.h"
#include "../../../EffekseerTool/EffekseerTool.Grid.Shader_PS.h"
	}

	LineRendererDX9::LineRendererDX9(EffekseerRenderer::Renderer* renderer)
	{
		this->renderer = (EffekseerRendererDX9::RendererImplemented*)renderer;

		// À•W(3) F(4)
		D3DVERTEXELEMENT9 decl[] =
		{
			{ 0,	0,	D3DDECLTYPE_FLOAT3,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_POSITION,	0 },
			{ 0,	12,	D3DDECLTYPE_FLOAT4,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_NORMAL,	0 },
			D3DDECL_END()
		};

		shader = EffekseerRendererDX9::Shader::Create(
			this->renderer,
			Shader_::g_vs20_VS,
			sizeof(Shader_::g_vs20_VS),
			Shader_::g_ps20_PS,
			sizeof(Shader_::g_ps20_PS), "Grid",
			decl);

		shader->SetVertexRegisterCount(4);
		shader->SetVertexConstantBufferSize(sizeof(float) * 16);
	}

	LineRendererDX9::~LineRendererDX9()
	{
		auto shader_ = (EffekseerRendererDX9::Shader*)shader;
		ES_SAFE_DELETE(shader_);
		shader = nullptr;
	}

	void LineRendererDX9::DrawLine(const Effekseer::Vector3D& p1, const Effekseer::Vector3D& p2, const Effekseer::Color& c)
	{
		Vertex v0;
		v0.Pos = p1;
		v0.Col[0] = c.R / 255.0f;
		v0.Col[1] = c.G / 255.0f;
		v0.Col[2] = c.B / 255.0f;
		v0.Col[3] = c.A / 255.0f;

		Vertex v1;
		v1.Pos = p2;
		v1.Col[0] = c.R / 255.0f;
		v1.Col[1] = c.G / 255.0f;
		v1.Col[2] = c.B / 255.0f;
		v1.Col[3] = c.A / 255.0f;

		vertexies.push_back(v0);
		vertexies.push_back(v1);
	}

	void LineRendererDX9::Render()
	{
		if (vertexies.size() == 0) return;

		renderer->GetVertexBuffer()->Lock();

		auto vs = (Vertex*)renderer->GetVertexBuffer()->GetBufferDirect(sizeof(Vertex) * vertexies.size());

		memcpy(vs, vertexies.data(), sizeof(Vertex) * vertexies.size());

		renderer->GetVertexBuffer()->Unlock();

		auto& state = renderer->GetRenderState()->Push();
		state.AlphaBlend = ::Effekseer::AlphaBlendType::Blend;
		state.DepthWrite = true;
		state.DepthTest = true;
		state.CullingType = Effekseer::CullingType::Double;

		renderer->BeginShader((EffekseerRendererDX9::Shader*)shader);

		auto mat = renderer->GetCameraProjectionMatrix();
		mat = mat.Transpose();
		memcpy(shader->GetVertexConstantBuffer(), &(mat), sizeof(float) * 16);

		shader->SetConstantBuffer();

		renderer->GetRenderState()->Update(false);

		renderer->SetLayout((EffekseerRendererDX9::Shader*)shader);
		renderer->GetDevice()->SetStreamSource(0, renderer->GetVertexBuffer()->GetInterface(), 0, sizeof(Vertex));
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
