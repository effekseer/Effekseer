#pragma once

#include <memory>

#include "../../efk.PostEffects.h"
#include <Effekseer.h>
#include <EffekseerRendererDX11/EffekseerRenderer/EffekseerRendererDX11.IndexBuffer.h>
#include <EffekseerRendererDX11/EffekseerRenderer/EffekseerRendererDX11.RenderState.h>
#include <EffekseerRendererDX11/EffekseerRenderer/EffekseerRendererDX11.Renderer.h>
#include <EffekseerRendererDX11/EffekseerRenderer/EffekseerRendererDX11.RendererImplemented.h>
#include <EffekseerRendererDX11/EffekseerRenderer/EffekseerRendererDX11.Shader.h>
#include <EffekseerRendererDX11/EffekseerRenderer/EffekseerRendererDX11.VertexBuffer.h>

namespace efk
{
class BlitterDX11
{
	struct Vertex
	{
		float x, y;
		float u, v;
	};

	Graphics* graphics;
	EffekseerRendererDX11::RendererImplemented* renderer_ = nullptr;
	std::unique_ptr<EffekseerRendererDX11::VertexBuffer> vertexBuffer;
	ID3D11SamplerState* sampler = nullptr;

public:
	BlitterDX11(Graphics* graphics, EffekseerRenderer::Renderer* renderer);
	virtual ~BlitterDX11();

	void Blit(EffekseerRendererDX11::Shader* shader,
			  int32_t numTextures,
			  ID3D11ShaderResourceView* const* textures,
			  const void* constantData,
			  size_t constantDataSize,
			  RenderTexture* dest,
			  Effekseer::AlphaBlendType blendType = Effekseer::AlphaBlendType::Opacity);
};

class BloomEffectDX11 : public BloomEffect
{
	static const int BlurBuffers = 2;
	static const int BlurIterations = 4;

	std::unique_ptr<EffekseerRendererDX11::Shader> shaderExtract;
	std::unique_ptr<EffekseerRendererDX11::Shader> shaderDownsample;
	std::unique_ptr<EffekseerRendererDX11::Shader> shaderBlend;
	std::unique_ptr<EffekseerRendererDX11::Shader> shaderBlurH;
	std::unique_ptr<EffekseerRendererDX11::Shader> shaderBlurV;

	BlitterDX11 blitter;
	Effekseer::Tool::Vector2DI renderTextureSize_;
	std::unique_ptr<RenderTexture> extractBuffer;
	std::unique_ptr<RenderTexture> lowresBuffers[BlurBuffers][BlurIterations];
	EffekseerRendererDX11::RendererImplemented* renderer_ = nullptr;

public:
	BloomEffectDX11(Graphics* graphics, EffekseerRenderer::Renderer* renderer);
	virtual ~BloomEffectDX11();

	void Render(RenderTexture* src, RenderTexture* dest) override;

	void OnLostDevice() override;

	void OnResetDevice() override;

private:
	void SetupBuffers(Effekseer::Tool::Vector2DI size);
	void ReleaseBuffers();
};

class TonemapEffectDX11 : public TonemapEffect
{
	std::unique_ptr<EffekseerRendererDX11::Shader> shaderCopy;
	std::unique_ptr<EffekseerRendererDX11::Shader> shaderReinhard;
	BlitterDX11 blitter;
	EffekseerRendererDX11::RendererImplemented* renderer_ = nullptr;

public:
	TonemapEffectDX11(Graphics* graphics, EffekseerRenderer::Renderer* renderer);
	virtual ~TonemapEffectDX11();

	void Render(RenderTexture* src, RenderTexture* dest) override;

	void OnLostDevice() override
	{
	}

	void OnResetDevice() override
	{
	}
};

class LinearToSRGBEffectDX11 : public LinearToSRGBEffect
{
	std::unique_ptr<EffekseerRendererDX11::Shader> shader_;
	BlitterDX11 blitter_;
	EffekseerRendererDX11::RendererImplemented* renderer_ = nullptr;

public:
	LinearToSRGBEffectDX11(Graphics* graphics, EffekseerRenderer::Renderer* renderer);
	virtual ~LinearToSRGBEffectDX11();

	void Render(RenderTexture* src, RenderTexture* dest) override;

	void OnLostDevice() override
	{
	}

	void OnResetDevice() override
	{
	}
};
} // namespace efk
