#include "EffekseerRendererArea.RenderState.h"

namespace EffekseerRendererArea
{
RenderState::RenderState() {}

RenderState::~RenderState() {}

void RenderState::Update(bool forced) { m_active = m_next; }
} // namespace EffekseerRendererArea
