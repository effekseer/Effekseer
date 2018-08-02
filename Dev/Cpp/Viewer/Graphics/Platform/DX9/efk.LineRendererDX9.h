
#pragma once

#include <Effekseer.h>
#include <EffekseerRendererDX9/EffekseerRenderer/EffekseerRendererDX9.Renderer.h>
#include <EffekseerRendererDX9/EffekseerRenderer/EffekseerRendererDX9.RendererImplemented.h>
#include <EffekseerRendererDX9/EffekseerRenderer/EffekseerRendererDX9.VertexBuffer.h>
#include <EffekseerRendererDX9/EffekseerRenderer/EffekseerRendererDX9.IndexBuffer.h>
#include <EffekseerRendererDX9/EffekseerRenderer/EffekseerRendererDX9.Shader.h>
#include <EffekseerRendererDX9/EffekseerRenderer/EffekseerRendererDX9.RenderState.h>

#include "../../efk.LineRenderer.h"

namespace efk
{
	class LineRendererDX9
		: public LineRenderer
	{
	private:
		EffekseerRendererDX9::RendererImplemented*	renderer = nullptr;
		EffekseerRenderer::ShaderBase*				shader = nullptr;

		std::vector<EffekseerRendererDX9::Vertex>	vertexies;
	public:

		LineRendererDX9(EffekseerRenderer::Renderer* renderer);
		virtual ~LineRendererDX9();

		void DrawLine(const Effekseer::Vector3D& p1, const Effekseer::Vector3D& p2, const Effekseer::Color& c) override;

		void Render() override;

		void ClearCache() override;

		void OnLostDevice() override;

		void OnResetDevice() override;
	};
}