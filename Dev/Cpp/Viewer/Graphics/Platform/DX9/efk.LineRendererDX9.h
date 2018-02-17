
#pragma once

#include <Effekseer.h>
#include <EffekseerRenderer/EffekseerRendererDX9.Renderer.h>
#include <EffekseerRenderer/EffekseerRendererDX9.RendererImplemented.h>
#include <EffekseerRenderer/EffekseerRendererDX9.VertexBuffer.h>
#include <EffekseerRenderer/EffekseerRendererDX9.IndexBuffer.h>
#include <EffekseerRenderer/EffekseerRendererDX9.Shader.h>
#include <EffekseerRenderer/EffekseerRendererDX9.RenderState.h>

namespace efk
{
	class LineRendererDX9
	{
	private:
		struct Vertex
		{
			::Effekseer::Vector3D	Pos;
			float	Col[4];
		};

		EffekseerRendererDX9::RendererImplemented*	renderer = nullptr;
		EffekseerRenderer::ShaderBase*				shader = nullptr;

		std::vector<Vertex>	vertexies;
	public:

		LineRendererDX9(EffekseerRenderer::Renderer* renderer);
		virtual ~LineRendererDX9();

		void DrawLine(const Effekseer::Vector3D& p1, const Effekseer::Vector3D& p2, const Effekseer::Color& c);

		void Render();

		void ClearCache();

		void OnLostDevice();

		void OnResetDevice();
	};
}