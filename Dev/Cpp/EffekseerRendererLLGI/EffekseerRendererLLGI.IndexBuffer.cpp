
#include "EffekseerRendererLLGI.IndexBuffer.h"

namespace EffekseerRendererLLGI
{

IndexBuffer::IndexBuffer(GraphicsDevice* graphicsDevice, LLGI::IndexBuffer* buffer, int maxCount, bool isDynamic, bool hasRefCount)
	: DeviceObject(graphicsDevice, hasRefCount)
	, IndexBufferBase(maxCount, isDynamic)
	, indexBuffer(buffer)
{
}

IndexBuffer::~IndexBuffer()
{
	LLGI::SafeRelease(indexBuffer);
}

IndexBuffer* IndexBuffer::Create(GraphicsDevice* graphicsDevice, int maxCount, bool isDynamic, bool hasRefCount)
{
	auto indexBuffer = graphicsDevice->GetGraphics()->CreateIndexBuffer(2, maxCount);
	if (indexBuffer == nullptr)
		return nullptr;

	return new IndexBuffer(graphicsDevice, indexBuffer, maxCount, isDynamic, hasRefCount);
}

void IndexBuffer::Lock()
{
	assert(!m_isLock);

	m_isLock = true;
	m_resource = (uint8_t*)indexBuffer->Lock();
	m_indexCount = 0;
}

void IndexBuffer::Unlock()
{
	assert(m_isLock);
	indexBuffer->Unlock();
	m_resource = NULL;
	m_isLock = false;
}

} // namespace EffekseerRendererLLGI
