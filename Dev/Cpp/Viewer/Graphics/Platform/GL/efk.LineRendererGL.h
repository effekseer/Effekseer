
#pragma once

#include <Effekseer.h>
#include <EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.IndexBuffer.h>
#include <EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.RenderState.h>
#include <EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.Renderer.h>
#include <EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.RendererImplemented.h>
#include <EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.Shader.h>
#include <EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.VertexArray.h>
#include <EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.VertexBuffer.h>

#include "../../efk.LineRenderer.h"

namespace efk
{
class LineRendererGL : public LineRenderer
{
private:
	EffekseerRendererGL::RendererImplemented* renderer = nullptr;
	EffekseerRenderer::ShaderBase* shader = nullptr;
	EffekseerRenderer::VertexBufferBase* vertexBuffer = nullptr;
	EffekseerRendererGL::VertexArray* vao = nullptr;

	std::vector<EffekseerRendererGL::Vertex> vertexies;

public:
	LineRendererGL(EffekseerRenderer::Renderer* renderer);
	virtual ~LineRendererGL();

	void DrawLine(const Effekseer::Vector3D& p1, const Effekseer::Vector3D& p2, const Effekseer::Color& c) override;

	void Render() override;

	void ClearCache() override;

	void OnLostDevice() override;

	void OnResetDevice() override;
};
} // namespace efk
