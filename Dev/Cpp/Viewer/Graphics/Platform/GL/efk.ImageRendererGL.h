#pragma once

#include <Effekseer.h>
#include <EffekseerRenderer/EffekseerRendererGL.Renderer.h>
#include <EffekseerRenderer/EffekseerRendererGL.RendererImplemented.h>
#include <EffekseerRenderer/EffekseerRendererGL.VertexBuffer.h>
#include <EffekseerRenderer/EffekseerRendererGL.IndexBuffer.h>
#include <EffekseerRenderer/EffekseerRendererGL.Shader.h>
#include <EffekseerRenderer/EffekseerRendererGL.RenderState.h>
#include <EffekseerRenderer/EffekseerRendererGL.VertexArray.h>
#include "../../efk.ImageRenderer.h"

namespace efk
{
	class ImageRendererGL
		: public ImageRenderer
	{
	private:
		struct Sprite
		{
			std::array<EffekseerRendererGL::Vertex, 4>	Verteies;
			void*	TexturePtr = nullptr;
		};

		EffekseerRendererGL::RendererImplemented*	renderer = nullptr;
		EffekseerRenderer::ShaderBase*				shader = nullptr;
		EffekseerRenderer::VertexBufferBase*		vertexBuffer = nullptr;
		EffekseerRenderer::ShaderBase*				shader_no_texture = nullptr;
        EffekseerRendererGL::VertexArray*           vao = nullptr;
        EffekseerRendererGL::VertexArray*           vao_nt = nullptr;

		std::vector<Sprite>	sprites;

	public:
		ImageRendererGL(EffekseerRenderer::Renderer* renderer);

		virtual ~ImageRendererGL();

		void Draw(const Effekseer::Vector3D positions[], const Effekseer::Vector2D uvs[], const Effekseer::Color colors[], void* texturePtr) override;

		void Render() override;

		void ClearCache() override;

		void OnLostDevice() override;

		void OnResetDevice() override;
	};
}
