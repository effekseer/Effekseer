
#pragma once

#include <Effekseer.h>
#include "../../EffekseerRendererCommon/EffekseerRenderer.Renderer.h"
#include "efk.Graphics.h"

namespace efk
{
	class LineRenderer
	{
	public:

		LineRenderer(EffekseerRenderer::Renderer* renderer) {}
		virtual ~LineRenderer() {}

		virtual void DrawLine(const Effekseer::Vector3D& p1, const Effekseer::Vector3D& p2, const Effekseer::Color& c) = 0;

		virtual void Render() = 0;

		virtual void ClearCache() = 0;

		virtual void OnLostDevice() = 0;

		virtual void OnResetDevice() = 0;

		static LineRenderer* Create(Graphics* graphics);
	};
}