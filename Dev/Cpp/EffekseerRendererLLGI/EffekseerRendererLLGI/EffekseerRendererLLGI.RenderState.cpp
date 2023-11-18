

#include "EffekseerRendererLLGI.RenderState.h"

namespace EffekseerRendererLLGI
{

RenderState::RenderState(RendererImplemented* renderer)
	: m_renderer(renderer)
{
}

RenderState::~RenderState()
{
}

void RenderState::Update(bool forced)
{
	bool changeDepth = forced;
	bool changeRasterizer = forced;
	bool changeBlend = forced;

	if (m_active.DepthTest != m_next.DepthTest || forced)
	{
		changeDepth = true;
	}

	if (m_active.DepthWrite != m_next.DepthWrite || forced)
	{
		changeDepth = true;
	}

	if (changeDepth)
	{
	}

	if (m_active.CullingType != m_next.CullingType || forced)
	{
		changeRasterizer = true;
	}

	if (changeRasterizer)
	{
		auto cullingType = (int32_t)m_next.CullingType;
	}

	if (m_active.AlphaBlend != m_next.AlphaBlend || forced)
	{
		changeBlend = true;
	}

	if (changeBlend)
	{
		auto alphaBlend = (int32_t)m_next.AlphaBlend;
	}

	for (int32_t i = 0; i < Effekseer::TextureSlotMax; i++)
	{
		bool changeSampler = forced;

		if (m_active.TextureFilterTypes[i] != m_next.TextureFilterTypes[i] || forced)
		{
			changeSampler = true;
		}

		if (m_active.TextureWrapTypes[i] != m_next.TextureWrapTypes[i] || forced)
		{
			changeSampler = true;
		}

		if (changeSampler)
		{
			auto filter = (int32_t)m_next.TextureFilterTypes[i];
			auto wrap = (int32_t)m_next.TextureWrapTypes[i];
		}
	}

	m_active = m_next;
}

} // namespace EffekseerRendererLLGI
