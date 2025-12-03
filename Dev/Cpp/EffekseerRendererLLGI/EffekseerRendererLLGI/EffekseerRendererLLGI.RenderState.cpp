

#include "EffekseerRendererLLGI.RenderState.h"

namespace EffekseerRendererLLGI
{

RenderState::RenderState(RendererImplemented* renderer)
	: renderer_(renderer)
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

	if (active_.DepthTest != next_.DepthTest || forced)
	{
		changeDepth = true;
	}

	if (active_.DepthWrite != next_.DepthWrite || forced)
	{
		changeDepth = true;
	}

	if (changeDepth)
	{
	}

	if (active_.CullingType != next_.CullingType || forced)
	{
		changeRasterizer = true;
	}

	if (changeRasterizer)
	{
		auto cullingType = (int32_t)next_.CullingType;
	}

	if (active_.AlphaBlend != next_.AlphaBlend || forced)
	{
		changeBlend = true;
	}

	if (changeBlend)
	{
		auto alphaBlend = (int32_t)next_.AlphaBlend;
	}

	for (int32_t i = 0; i < Effekseer::TextureSlotMax; i++)
	{
		bool changeSampler = forced;

		if (active_.TextureFilterTypes[i] != next_.TextureFilterTypes[i] || forced)
		{
			changeSampler = true;
		}

		if (active_.TextureWrapTypes[i] != next_.TextureWrapTypes[i] || forced)
		{
			changeSampler = true;
		}

		if (changeSampler)
		{
			auto filter = (int32_t)next_.TextureFilterTypes[i];
			auto wrap = (int32_t)next_.TextureWrapTypes[i];
		}
	}

	active_ = next_;
}

} // namespace EffekseerRendererLLGI
