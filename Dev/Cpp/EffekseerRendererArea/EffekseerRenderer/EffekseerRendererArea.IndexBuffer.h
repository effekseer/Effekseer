#pragma once

#include "../EffekseerRendererCommon/EffekseerRenderer.IndexBufferBase.h"

namespace EffekseerRendererArea
{
class IndexBuffer : public EffekseerRenderer::IndexBufferBase
{
public:
	IndexBuffer(int maxCount, bool isDynamic) : EffekseerRenderer::IndexBufferBase(maxCount, isDynamic) {}

	virtual ~IndexBuffer() {}

	void Lock() override {}
	void Unlock() override {}
};
} // namespace EffekseerRendererArea