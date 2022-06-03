
#pragma once

#include "../EffekseerRendererCommon/EffekseerRenderer.IndexBufferBase.h"
#include "EffekseerRendererLLGI.DeviceObject.h"
#include "EffekseerRendererLLGI.RendererImplemented.h"
#include <LLGI.Buffer.h>

namespace EffekseerRendererLLGI
{

class IndexBuffer : public DeviceObject, public ::EffekseerRenderer::IndexBufferBase
{
private:
	LLGI::Buffer* indexBuffer = nullptr;

	IndexBuffer(Backend::GraphicsDevice* graphicsDevice, LLGI::Buffer* buffer, int maxCount, bool isDynamic, bool hasRefCount);

public:
	virtual ~IndexBuffer();

	static IndexBuffer* Create(Backend::GraphicsDevice* graphicsDevice, int maxCount, bool isDynamic, bool hasRefCount);

	LLGI::Buffer* GetIndexBuffer()
	{
		return indexBuffer;
	}

public:
	void Lock();
	void Unlock();
};

} // namespace EffekseerRendererLLGI
