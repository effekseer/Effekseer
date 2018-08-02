#pragma once

#include <Effekseer.h>
#include <EffekseerRendererDX9/EffekseerRenderer/EffekseerRendererDX9.Renderer.h>
#include <EffekseerRendererDX9/EffekseerRenderer/EffekseerRendererDX9.RendererImplemented.h>
#include <EffekseerRendererDX9/EffekseerRenderer/EffekseerRendererDX9.VertexBuffer.h>
#include <EffekseerRendererDX9/EffekseerRenderer/EffekseerRendererDX9.IndexBuffer.h>
#include <EffekseerRendererDX9/EffekseerRenderer/EffekseerRendererDX9.Shader.h>
#include <EffekseerRendererDX9/EffekseerRenderer/EffekseerRendererDX9.RenderState.h>
#include "../../efk.ImageRenderer.h"

namespace efk
{
	class ImageRendererDX9
		: public ImageRenderer
	{
	private:
		struct Sprite
		{
			std::array<EffekseerRendererDX9::Vertex, 4>	Verteies;
			void*	TexturePtr = nullptr;
		};

		EffekseerRendererDX9::RendererImplemented*	renderer = nullptr;
		EffekseerRenderer::ShaderBase*				shader = nullptr;
		EffekseerRenderer::ShaderBase*				shader_no_texture = nullptr;

		std::vector<Sprite>	sprites;

	public:
		ImageRendererDX9(EffekseerRenderer::Renderer* renderer);

		virtual ~ImageRendererDX9();

		void Draw(const Effekseer::Vector3D positions[], const Effekseer::Vector2D uvs[], const Effekseer::Color colors[], void* texturePtr) override;

		void Render() override;

		void ClearCache() override;

		void OnLostDevice() override;

		void OnResetDevice() override;
	};
}