#pragma once

#include "../EffekseerRendererCommon/EffekseerRenderer.RenderStateBase.h"

namespace EffekseerRendererArea
{
class RenderState : public EffekseerRenderer::RenderStateBase
{
public:
	RenderState();
	~RenderState();
	void Update(bool forced);
};
} // namespace EffekseerRendererArea
