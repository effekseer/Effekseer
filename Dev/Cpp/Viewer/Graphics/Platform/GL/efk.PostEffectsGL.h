#pragma once

#include "../../efk.PostEffects.h"
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
class BlitterGL
{
	Graphics* graphics = nullptr;
	EffekseerRendererGL::RendererImplementedRef renderer_;

	std::unique_ptr<EffekseerRendererGL::VertexBuffer> vertexBuffer;

public:
	struct Vertex
	{
		float x, y;
		float u, v;
	};

	BlitterGL(Graphics* graphics, const EffekseerRenderer::RendererRef& renderer);
	virtual ~BlitterGL();

	std::unique_ptr<EffekseerRendererGL::VertexArray> CreateVAO(Effekseer::Backend::GraphicsDeviceRef graphicsDevice, EffekseerRendererGL::Shader* shader);

	void Blit(EffekseerRendererGL::Shader* shader,
			  EffekseerRendererGL::VertexArray* vao,
			  const std::vector<Effekseer::Backend::TextureRef>& textures,
			  const void* constantData,
			  size_t constantDataSize,
			  Effekseer::Backend::TextureRef dest,
			  bool isCleared = true);
};

class BloomEffectGL : public BloomEffect
{
	static const int BlurBuffers = 2;
	static const int BlurIterations = 4;

	EffekseerRendererGL::RendererImplementedRef renderer_;
	BlitterGL blitter;

	std::unique_ptr<EffekseerRendererGL::Shader> shaderExtract;
	std::unique_ptr<EffekseerRendererGL::Shader> shaderDownsample;
	std::unique_ptr<EffekseerRendererGL::Shader> shaderBlend;
	std::unique_ptr<EffekseerRendererGL::Shader> shaderBlurH;
	std::unique_ptr<EffekseerRendererGL::Shader> shaderBlurV;

	std::unique_ptr<EffekseerRendererGL::VertexArray> vaoExtract;
	std::unique_ptr<EffekseerRendererGL::VertexArray> vaoDownsample;
	std::unique_ptr<EffekseerRendererGL::VertexArray> vaoBlend;
	std::unique_ptr<EffekseerRendererGL::VertexArray> vaoBlurH;
	std::unique_ptr<EffekseerRendererGL::VertexArray> vaoBlurV;

	int32_t renderTextureWidth = 0;
	int32_t renderTextureHeight = 0;
	Effekseer::Backend::TextureRef extractBuffer;
	Effekseer::Backend::TextureRef lowresBuffers[BlurBuffers][BlurIterations];

public:
	BloomEffectGL(Graphics* graphics, const EffekseerRenderer::RendererRef& renderer);
	virtual ~BloomEffectGL();

	void Render(Effekseer::Backend::TextureRef src, Effekseer::Backend::TextureRef dest) override;

	void OnLostDevice() override;

	void OnResetDevice() override;

private:
	void SetupBuffers(int32_t width, int32_t height);
	void ReleaseBuffers();
};

} // namespace efk
