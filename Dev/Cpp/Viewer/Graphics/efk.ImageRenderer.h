#pragma once

#include <Effekseer.h>
#include "../../EffekseerRendererCommon/EffekseerRenderer.Renderer.h"
#include "efk.Graphics.h"

namespace efk
{
	class ImageRenderer
	{
	public:
		ImageRenderer(EffekseerRenderer::Renderer* renderer) {}

		virtual ~ImageRenderer() {}

		virtual void Draw(const Effekseer::Vector3D positions[], const Effekseer::Vector2D uvs[], const Effekseer::Color colors[], ::Effekseer::TextureData* texturePtr) = 0;

		virtual void Render() = 0;

		virtual void ClearCache() = 0;

		virtual void OnLostDevice() = 0;

		virtual void OnResetDevice() = 0;

		static ImageRenderer* Create(Graphics* graphics);
	};
}