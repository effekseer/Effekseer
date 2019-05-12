
#pragma once

#include "../EffekseerRendererCommon/EffekseerRenderer.IndexBufferBase.h"
#include "EffekseerRendererLLGI.DeviceObject.h"
#include "EffekseerRendererLLGI.RendererImplemented.h"
#include <LLGI.IndexBuffer.h>

namespace EffekseerRendererLLGI
{

class IndexBuffer : public DeviceObject, public ::EffekseerRenderer::IndexBufferBase
{
private:
	LLGI::IndexBuffer* indexBuffer = nullptr;

	IndexBuffer(RendererImplemented* renderer, LLGI::IndexBuffer* buffer, int maxCount, bool isDynamic);

public:
	virtual ~IndexBuffer();

	static IndexBuffer* Create(RendererImplemented* renderer, int maxCount, bool isDynamic);

	LLGI::IndexBuffer* GetIndexBuffer() { return indexBuffer; }

public:
	void Lock();
	void Unlock();
};

} // namespace EffekseerRendererLLGI
