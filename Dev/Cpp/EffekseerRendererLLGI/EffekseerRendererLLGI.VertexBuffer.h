
#pragma once

#include "../EffekseerRendererCommon/EffekseerRenderer.VertexBufferBase.h"
#include "EffekseerRendererLLGI.DeviceObject.h"
#include "EffekseerRendererLLGI.RendererImplemented.h"
#include <LLGI.VertexBuffer.h>

namespace EffekseerRendererLLGI
{

class VertexBuffer : public DeviceObject, public ::EffekseerRenderer::VertexBufferBase
{
protected:
	// TODO make ring buffer
	int32_t currentIndex = 0;
	std::vector<LLGI::VertexBuffer*> vertexBuffers;

	Effekseer::CustomAlignedVector<uint8_t> lockedResource_;

	bool m_ringBufferLock;

	int32_t m_ringLockedOffset;
	int32_t m_ringLockedSize;

	VertexBuffer(Backend::GraphicsDevice* graphicsDevice, LLGI::VertexBuffer* buffer, int size, bool isDynamic, bool hasRefCount);

public:
	virtual ~VertexBuffer();

	static VertexBuffer* Create(Backend::GraphicsDevice* graphicsDevice, int size, bool isDynamic, bool hasRefCount);

	LLGI::VertexBuffer* GetVertexBuffer()
	{
		return vertexBuffers[currentIndex];
	}

public:
	void Lock() override;
	bool RingBufferLock(int32_t size, int32_t& offset, void*& data, int32_t alignment) override;
	bool TryRingBufferLock(int32_t size, int32_t& offset, void*& data, int32_t alignment) override;
	void Unlock() override;
};

} // namespace EffekseerRendererLLGI
