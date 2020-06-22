
#pragma once

#include <Effekseer.h>
#include <EffekseerRendererDX11/EffekseerRenderer/EffekseerRendererDX11.IndexBuffer.h>
#include <EffekseerRendererDX11/EffekseerRenderer/EffekseerRendererDX11.RenderState.h>
#include <EffekseerRendererDX11/EffekseerRenderer/EffekseerRendererDX11.Renderer.h>
#include <EffekseerRendererDX11/EffekseerRenderer/EffekseerRendererDX11.RendererImplemented.h>
#include <EffekseerRendererDX11/EffekseerRenderer/EffekseerRendererDX11.Shader.h>
#include <EffekseerRendererDX11/EffekseerRenderer/EffekseerRendererDX11.VertexBuffer.h>

#include "../../efk.LineRenderer.h"

namespace efk
{
class LineRendererDX11 : public LineRenderer
{
private:
	EffekseerRendererDX11::RendererImplemented* renderer = nullptr;
	EffekseerRenderer::ShaderBase* shader = nullptr;

	std::vector<EffekseerRendererDX11::Vertex> vertexies;

public:
	LineRendererDX11(EffekseerRenderer::Renderer* renderer);
	virtual ~LineRendererDX11();

	void DrawLine(const Effekseer::Vector3D& p1, const Effekseer::Vector3D& p2, const Effekseer::Color& c) override;

	void Render() override;

	void ClearCache() override;

	void OnLostDevice() override;

	void OnResetDevice() override;
};
} // namespace efk