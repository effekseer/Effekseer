
#include "EffekseerRendererLLGI.IndexBuffer.h"

namespace EffekseerRendererLLGI
{

IndexBuffer::IndexBuffer(RendererImplemented* renderer, LLGI::IndexBuffer* buffer, int maxCount, bool isDynamic)
	: DeviceObject(renderer), IndexBufferBase(maxCount, isDynamic), indexBuffer(buffer)
{
}

IndexBuffer::~IndexBuffer() { LLGI::SafeRelease(indexBuffer); }

IndexBuffer* IndexBuffer::Create(RendererImplemented* renderer, int maxCount, bool isDynamic)
{
	auto indexBuffer = renderer->GetGraphics()->CreateIndexBuffer(2, maxCount);
	if (indexBuffer == nullptr)
		return nullptr;

	return new IndexBuffer(renderer, indexBuffer, maxCount, isDynamic);
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
