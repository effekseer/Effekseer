
#pragma once

#include "../EffekseerRendererCommon/EffekseerRenderer.VertexBufferBase.h"
#include "EffekseerRendererLLGI.DeviceObject.h"
#include "EffekseerRendererLLGI.RendererImplemented.h"
#include <LLGI.Buffer.h>

namespace EffekseerRendererLLGI
{

class VertexBuffer : public DeviceObject, public ::EffekseerRenderer::VertexBufferBase
{
protected:
	// TODO make correct ring buffer
	const int ringVertexCount_ = 3;

	int32_t nextIndex_ = 0;

	Effekseer::CustomAlignedVector<uint8_t> lockedResource_;

	bool m_ringBufferLock;

	int32_t m_ringLockedOffset;
	int32_t m_ringLockedSize;

	std::vector<std::shared_ptr<LLGI::Buffer>> storedVertexBuffers_;
	std::shared_ptr<LLGI::Buffer> CreateBuffer();
	std::shared_ptr<LLGI::Buffer> GetNextBuffer();

	std::shared_ptr<LLGI::Buffer> currentVertexBuffer_;

	bool Init();
	VertexBuffer(Backend::GraphicsDevice* graphicsDevice, int size, bool isDynamic, bool hasRefCount);

public:
	~VertexBuffer() override = default;

	static VertexBuffer* Create(Backend::GraphicsDevice* graphicsDevice, int size, bool isDynamic, bool hasRefCount);

	LLGI::Buffer* GetVertexBuffer()
	{
		return currentVertexBuffer_.get();
	}

public:
	void Lock() override;
	bool RingBufferLock(int32_t size, int32_t& offset, void*& data, int32_t alignment) override;
	bool TryRingBufferLock(int32_t size, int32_t& offset, void*& data, int32_t alignment) override;
	void Unlock() override;
};

} // namespace EffekseerRendererLLGI
