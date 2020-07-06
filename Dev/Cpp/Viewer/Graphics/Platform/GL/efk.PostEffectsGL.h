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
	EffekseerRendererGL::RendererImplemented* renderer_ = nullptr;

	std::unique_ptr<EffekseerRendererGL::VertexBuffer> vertexBuffer;

public:
	struct Vertex
	{
		float x, y;
		float u, v;
	};
	static const EffekseerRendererGL::ShaderAttribInfo shaderAttributes[2];

	BlitterGL(Graphics* graphics, EffekseerRenderer::Renderer* renderer);
	virtual ~BlitterGL();

	std::unique_ptr<EffekseerRendererGL::VertexArray> CreateVAO(EffekseerRendererGL::Shader* shader);

	void Blit(EffekseerRendererGL::Shader* shader,
			  EffekseerRendererGL::VertexArray* vao,
			  int32_t numTextures,
			  const GLuint* textures,
			  const void* constantData,
			  size_t constantDataSize,
			  RenderTexture* dest);
};

class BloomEffectGL : public BloomEffect
{
	static const int BlurBuffers = 2;
	static const int BlurIterations = 4;

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
	std::unique_ptr<RenderTexture> extractBuffer;
	std::unique_ptr<RenderTexture> lowresBuffers[BlurBuffers][BlurIterations];
	EffekseerRendererGL::RendererImplemented* renderer_ = nullptr;

public:
	BloomEffectGL(Graphics* graphics, EffekseerRenderer::Renderer* renderer);
	virtual ~BloomEffectGL();

	void Render(RenderTexture* src, RenderTexture* dest) override;

	void OnLostDevice() override;

	void OnResetDevice() override;

private:
	void SetupBuffers(int32_t width, int32_t height);
	void ReleaseBuffers();
};

class TonemapEffectGL : public TonemapEffect
{
	BlitterGL blitter;

	std::unique_ptr<EffekseerRendererGL::Shader> shaderCopy;
	std::unique_ptr<EffekseerRendererGL::Shader> shaderReinhard;

	std::unique_ptr<EffekseerRendererGL::VertexArray> vaoCopy;
	std::unique_ptr<EffekseerRendererGL::VertexArray> vaoReinhard;
	EffekseerRendererGL::RendererImplemented* renderer_ = nullptr;

public:
	TonemapEffectGL(Graphics* graphics, EffekseerRenderer::Renderer* renderer);
	virtual ~TonemapEffectGL();

	void Render(RenderTexture* src, RenderTexture* dest) override;

	void OnLostDevice() override
	{
	}

	void OnResetDevice() override
	{
	}
};

class LinearToSRGBEffectGL : public LinearToSRGBEffect
{
	BlitterGL blitter;

	std::unique_ptr<EffekseerRendererGL::Shader> shader_;
	std::unique_ptr<EffekseerRendererGL::VertexArray> vao_;
	EffekseerRendererGL::RendererImplemented* renderer_ = nullptr;

public:
	LinearToSRGBEffectGL(Graphics* graphics, EffekseerRenderer::Renderer* renderer);
	virtual ~LinearToSRGBEffectGL();

	void Render(RenderTexture* src, RenderTexture* dest) override;

	void OnLostDevice() override
	{
	}

	void OnResetDevice() override
	{
	}
};
} // namespace efk
