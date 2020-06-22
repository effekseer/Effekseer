#pragma once

#include "../../efk.ImageRenderer.h"
#include <Effekseer.h>
#include <EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.IndexBuffer.h>
#include <EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.RenderState.h>
#include <EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.Renderer.h>
#include <EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.RendererImplemented.h>
#include <EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.Shader.h>
#include <EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.VertexArray.h>
#include <EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.VertexBuffer.h>

namespace efk
{
class ImageRendererGL : public ImageRenderer
{
private:
	struct Sprite
	{
		std::array<EffekseerRendererGL::Vertex, 4> Verteies;
		::Effekseer::TextureData* TexturePtr = nullptr;
	};

	EffekseerRendererGL::RendererImplemented* renderer = nullptr;
	EffekseerRenderer::ShaderBase* shader = nullptr;
	EffekseerRenderer::VertexBufferBase* vertexBuffer = nullptr;
	EffekseerRenderer::ShaderBase* shader_no_texture = nullptr;
	EffekseerRendererGL::VertexArray* vao = nullptr;
	EffekseerRendererGL::VertexArray* vao_nt = nullptr;

	std::vector<Sprite> sprites;

public:
	ImageRendererGL(EffekseerRenderer::Renderer* renderer);

	virtual ~ImageRendererGL();

	void Draw(const Effekseer::Vector3D positions[],
			  const Effekseer::Vector2D uvs[],
			  const Effekseer::Color colors[],
			  ::Effekseer::TextureData* texturePtr) override;

	void Render() override;

	void ClearCache() override;

	void OnLostDevice() override;

	void OnResetDevice() override;
};
} // namespace efk
