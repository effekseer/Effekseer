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
	EffekseerRendererDX11::RendererImplementedRef renderer_;
	std::unique_ptr<EffekseerRendererDX11::VertexBuffer> vertexBuffer;
	ID3D11SamplerState* sampler = nullptr;

public:
	BlitterDX11(Graphics* graphics, const EffekseerRenderer::RendererRef& renderer);
	virtual ~BlitterDX11();

	void Blit(EffekseerRendererDX11::Shader* shader,
			  const std::vector<Effekseer::Backend::TextureRef>& textures,
			  const void* constantData,
			  size_t constantDataSize,
			  Effekseer::Backend::TextureRef dest,
			  Effekseer::AlphaBlendType blendType = Effekseer::AlphaBlendType::Opacity);
};

class BloomEffectDX11 : public BloomEffect
{
	static const int BlurBuffers = 2;
	static const int BlurIterations = 4;

	EffekseerRendererDX11::RendererImplementedRef renderer_;
	BlitterDX11 blitter;

	std::unique_ptr<EffekseerRendererDX11::Shader> shaderExtract;
	std::unique_ptr<EffekseerRendererDX11::Shader> shaderDownsample;
	std::unique_ptr<EffekseerRendererDX11::Shader> shaderBlend;
	std::unique_ptr<EffekseerRendererDX11::Shader> shaderBlurH;
	std::unique_ptr<EffekseerRendererDX11::Shader> shaderBlurV;

	std::array<int32_t, 2> renderTextureSize_;
	std::unique_ptr<RenderTexture> extractBuffer;
	std::unique_ptr<RenderTexture> lowresBuffers[BlurBuffers][BlurIterations];

public:
	BloomEffectDX11(Graphics* graphics, const EffekseerRenderer::RendererRef& renderer);
	virtual ~BloomEffectDX11();

	void Render(Effekseer::Backend::TextureRef src, Effekseer::Backend::TextureRef dest) override;

	void OnLostDevice() override;

	void OnResetDevice() override;

private:
	void SetupBuffers(std::array<int32_t, 2> size);
	void ReleaseBuffers();
};

class TonemapEffectDX11 : public TonemapEffect
{
	EffekseerRendererDX11::RendererImplementedRef renderer_;
	BlitterDX11 blitter;
	std::unique_ptr<EffekseerRendererDX11::Shader> shaderCopy;
	std::unique_ptr<EffekseerRendererDX11::Shader> shaderReinhard;

public:
	TonemapEffectDX11(Graphics* graphics, const EffekseerRenderer::RendererRef& renderer);
	virtual ~TonemapEffectDX11();

	void Render(Effekseer::Backend::TextureRef src, Effekseer::Backend::TextureRef dest) override;

	void OnLostDevice() override
	{
	}

	void OnResetDevice() override
	{
	}
};

class LinearToSRGBEffectDX11 : public LinearToSRGBEffect
{
	EffekseerRendererDX11::RendererImplementedRef renderer_;
	BlitterDX11 blitter_;
	std::unique_ptr<EffekseerRendererDX11::Shader> shader_;

public:
	LinearToSRGBEffectDX11(Graphics* graphics, const EffekseerRenderer::RendererRef& renderer);
	virtual ~LinearToSRGBEffectDX11();

	void Render(Effekseer::Backend::TextureRef src, Effekseer::Backend::TextureRef dest) override;

	void OnLostDevice() override
	{
	}

	void OnResetDevice() override
	{
	}
};
} // namespace efk
