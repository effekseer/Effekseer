
#pragma once

#include "../../EffekseerRendererCommon/EffekseerRenderer.Renderer.h"
#include "efk.Graphics.h"
#include <Effekseer.h>

namespace efk
{
class LineRenderer
{
public:
	LineRenderer(const EffekseerRenderer::RendererRef& renderer)
	{
	}
	virtual ~LineRenderer()
	{
	}

	virtual void DrawLine(const Effekseer::Vector3D& p1, const Effekseer::Vector3D& p2, const Effekseer::Color& c) = 0;

	virtual void Render() = 0;

	virtual void ClearCache() = 0;

	virtual void OnLostDevice() = 0;

	virtual void OnResetDevice() = 0;

	static LineRenderer* Create(Graphics* graphics, const EffekseerRenderer::RendererRef& renderer);
};
} // namespace efk