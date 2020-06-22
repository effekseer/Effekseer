#pragma once

#include <memory>

#include <Effekseer.h>
#include <EffekseerRendererDX11/EffekseerRenderer/EffekseerRendererDX11.Renderer.h>
#include <EffekseerRendererDX11/EffekseerRenderer/EffekseerRendererDX11.RendererImplemented.h>
#include <EffekseerRendererDX11/EffekseerRenderer/EffekseerRendererDX11.VertexBuffer.h>
#include <EffekseerRendererDX11/EffekseerRenderer/EffekseerRendererDX11.IndexBuffer.h>
#include <EffekseerRendererDX11/EffekseerRenderer/EffekseerRendererDX11.Shader.h>
#include <EffekseerRendererDX11/EffekseerRenderer/EffekseerRendererDX11.RenderState.h>
#include "../../efk.PostEffects.h"

namespace efk
{
	class BlitterDX11
	{
		struct Vertex {
			float x, y;
			float u, v;
		};

		Graphics* graphics;
		std::unique_ptr<EffekseerRendererDX11::VertexBuffer> vertexBuffer;
		ID3D11SamplerState* sampler = nullptr;

	public:
		BlitterDX11(Graphics* graphics);
		virtual ~BlitterDX11();

		void Blit(EffekseerRendererDX11::Shader* shader, 
			int32_t numTextures, ID3D11ShaderResourceView* const* textures, 
			const void* constantData, size_t constantDataSize, RenderTexture* dest, 
			Effekseer::AlphaBlendType blendType = Effekseer::AlphaBlendType::Opacity);
	};

	class BloomEffectDX11
		: public BloomEffect
	{
		static const int BlurBuffers = 2;
		static const int BlurIterations = 4;

		std::unique_ptr<EffekseerRendererDX11::Shader> shaderExtract;
		std::unique_ptr<EffekseerRendererDX11::Shader> shaderDownsample;
		std::unique_ptr<EffekseerRendererDX11::Shader> shaderBlend;
		std::unique_ptr<EffekseerRendererDX11::Shader> shaderBlurH;
		std::unique_ptr<EffekseerRendererDX11::Shader> shaderBlurV;

		BlitterDX11 blitter;
		int32_t renderTextureWidth = 0;
		int32_t renderTextureHeight = 0;
		std::unique_ptr<RenderTexture> extractBuffer;
		std::unique_ptr<RenderTexture> lowresBuffers[BlurBuffers][BlurIterations];

	public:
		BloomEffectDX11(Graphics* graphics);
		virtual ~BloomEffectDX11();

		void Render(RenderTexture* src, RenderTexture* dest) override;

		void OnLostDevice() override;

		void OnResetDevice() override;

	private:
		void SetupBuffers(int32_t width, int32_t height);
		void ReleaseBuffers();
	};

	class TonemapEffectDX11
		: public TonemapEffect
	{
		std::unique_ptr<EffekseerRendererDX11::Shader> shaderCopy;
		std::unique_ptr<EffekseerRendererDX11::Shader> shaderReinhard;
		BlitterDX11 blitter;

	public:
		TonemapEffectDX11(Graphics* graphics);
		virtual ~TonemapEffectDX11();

		void Render(RenderTexture* src, RenderTexture* dest) override;

		void OnLostDevice() override {}

		void OnResetDevice() override {}
	};

	class LinearToSRGBEffectDX11 : public LinearToSRGBEffect
	{
		std::unique_ptr<EffekseerRendererDX11::Shader> shader_;
		BlitterDX11 blitter_;

	public:
		LinearToSRGBEffectDX11(Graphics* graphics);
		virtual ~LinearToSRGBEffectDX11();

		void Render(RenderTexture* src, RenderTexture* dest) override;

		void OnLostDevice() override {}

		void OnResetDevice() override {}
	};
	}
