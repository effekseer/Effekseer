
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

	}
}
