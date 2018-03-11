
#pragma once

#include <Effekseer.h>
#include <EffekseerRenderer/EffekseerRendererGL.Renderer.h>
#include <EffekseerRenderer/EffekseerRendererGL.RendererImplemented.h>
#include <EffekseerRenderer/EffekseerRendererGL.VertexBuffer.h>
#include <EffekseerRenderer/EffekseerRendererGL.IndexBuffer.h>
#include <EffekseerRenderer/EffekseerRendererGL.Shader.h>
#include <EffekseerRenderer/EffekseerRendererGL.RenderState.h>
#include <EffekseerRenderer/EffekseerRendererGL.VertexArray.h>

#include "../../efk.LineRenderer.h"

namespace efk
{
	class LineRendererGL
		: public LineRenderer
	{
	private:
		EffekseerRendererGL::RendererImplemented*	renderer = nullptr;
		EffekseerRenderer::ShaderBase*				shader = nullptr;
        EffekseerRendererGL::VertexArray*           vao = nullptr;
        
		std::vector<EffekseerRendererGL::Vertex>	vertexies;
	public:

		LineRendererGL(EffekseerRenderer::Renderer* renderer);
		virtual ~LineRendererGL();

		void DrawLine(const Effekseer::Vector3D& p1, const Effekseer::Vector3D& p2, const Effekseer::Color& c) override;

		void Render() override;

		void ClearCache() override;

		void OnLostDevice() override;

		void OnResetDevice() override;
	};
}
