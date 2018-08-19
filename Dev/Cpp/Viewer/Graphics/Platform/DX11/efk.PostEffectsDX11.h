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
	class BloomEffectDX11
		: public PostEffect
	{
		static const int BlurBuffers = 2;
		static const int BlurIterations = 4;

		struct Vertex {
			float x, y;
			float u, v;
		};

		std::unique_ptr<EffekseerRendererDX11::Shader> shaderExtract;
		std::unique_ptr<EffekseerRendererDX11::Shader> shaderCopy;
		std::unique_ptr<EffekseerRendererDX11::Shader> shaderBlend;
		std::unique_ptr<EffekseerRendererDX11::Shader> shaderBlurH;
		std::unique_ptr<EffekseerRendererDX11::Shader> shaderBlurV;

		std::unique_ptr<EffekseerRendererDX11::VertexBuffer> vertexBuffer;
		ID3D11SamplerState* sampler = nullptr;

		int32_t renderTextureWidth = 0;
		int32_t renderTextureHeight = 0;
		std::unique_ptr<RenderTexture> extractBuffer;
		std::unique_ptr<RenderTexture> lowresBuffers[BlurBuffers][BlurIterations];

	public:
		BloomEffectDX11(Graphics* graphics);
		virtual ~BloomEffectDX11();

		void Render() override;

		void OnLostDevice() override;

		void OnResetDevice() override;

	private:
		void SetupBuffers(int32_t width, int32_t height);
		void ReleaseBuffers();
	};
}
