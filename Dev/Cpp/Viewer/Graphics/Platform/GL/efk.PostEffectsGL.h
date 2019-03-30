#pragma once

#include <Effekseer.h>
#include <EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.Renderer.h>
#include <EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.RendererImplemented.h>
#include <EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.VertexBuffer.h>
#include <EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.IndexBuffer.h>
#include <EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.Shader.h>
#include <EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.RenderState.h>
#include <EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.VertexArray.h>
#include "../../efk.PostEffects.h"

namespace efk
{
	class BloomEffectGL
		: public BloomEffect
	{
		static const int BlurBuffers = 2;
		static const int BlurIterations = 4;

		struct Vertex {
			float x, y;
			float u, v;
		};

		std::unique_ptr<EffekseerRendererGL::Shader> shaderExtract;
		std::unique_ptr<EffekseerRendererGL::Shader> shaderCopy;
		std::unique_ptr<EffekseerRendererGL::Shader> shaderBlend;
		std::unique_ptr<EffekseerRendererGL::Shader> shaderBlurH;
		std::unique_ptr<EffekseerRendererGL::Shader> shaderBlurV;

		std::unique_ptr<EffekseerRendererGL::VertexArray> vaoExtract;
		std::unique_ptr<EffekseerRendererGL::VertexArray> vaoCopy;
		std::unique_ptr<EffekseerRendererGL::VertexArray> vaoBlend;
		std::unique_ptr<EffekseerRendererGL::VertexArray> vaoBlurH;
		std::unique_ptr<EffekseerRendererGL::VertexArray> vaoBlurV;
		std::unique_ptr<EffekseerRendererGL::VertexBuffer> vertexBuffer;

		int32_t renderTextureWidth = 0;
		int32_t renderTextureHeight = 0;
		std::unique_ptr<RenderTexture> extractBuffer;
		std::unique_ptr<RenderTexture> lowresBuffers[BlurBuffers][BlurIterations];

	public:
		BloomEffectGL(Graphics* graphics);
		virtual ~BloomEffectGL();

		void Render(RenderTexture* src, RenderTexture* dest) override;

		void OnLostDevice() override;

		void OnResetDevice() override;

	private:
		void SetupBuffers(int32_t width, int32_t height);
		void ReleaseBuffers();
	};
}
