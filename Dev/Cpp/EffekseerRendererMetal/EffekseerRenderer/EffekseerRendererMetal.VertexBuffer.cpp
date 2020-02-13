
#include "EffekseerRendererMetal.VertexBuffer.h"

namespace EffekseerRendererMetal
{

VertexBuffer::VertexBuffer(::EffekseerRendererLLGI::GraphicsDevice* graphicsDevice, LLGI::VertexBuffer* buffer, int size, bool isDynamic, bool hasRefCount)
	: ::EffekseerRendererLLGI::VertexBuffer(graphicsDevice, buffer, size, isDynamic, hasRefCount)
{
    // create more buffers before-hand
    for (int i = 0; i < 2; ++i)
    {
        AddNewBuffer();
    }
}

void VertexBuffer::AddNewBuffer()
{
    auto vb = GetGraphicsDevice()->GetGraphics()->CreateVertexBuffer(m_size);
    if (vb == nullptr) return;
    
    vertexBuffers.push_back(vb);
}

VertexBuffer* VertexBuffer::Create(::EffekseerRendererLLGI::GraphicsDevice* graphicsDevice, int size, bool isDynamic, bool hasRefCount)
{
	auto vertexBuffer = graphicsDevice->GetGraphics()->CreateVertexBuffer(size);
	if (vertexBuffer == nullptr)
		return nullptr;

	return new VertexBuffer(graphicsDevice, vertexBuffer, size, isDynamic, hasRefCount);
}

bool VertexBuffer::RingBufferLock(int32_t size, int32_t& offset, void*& data, int32_t alignment)
{
	assert(!m_isLock);
	assert(!m_ringBufferLock);
	assert(this->m_isDynamic);

	if (size > m_size)
		return false;

	m_vertexRingOffset = (m_vertexRingOffset + alignment - 1) / alignment* alignment;

	if ((int32_t)m_vertexRingOffset + size > m_size)
	{
        currentIndex++;
        if (currentIndex > vertexBuffers.size() - 1)
        { AddNewBuffer(); }
        
        offset = 0;
        m_ringLockedOffset = 0;
        m_ringLockedSize = size;

        m_vertexRingOffset = size;
	}
	else
	{
		offset = m_vertexRingOffset;
		m_ringLockedOffset = offset;
		m_ringLockedSize = size;

		m_vertexRingOffset += size;
	}

	data = (uint8_t*)lockedResource_;
	m_resource = (uint8_t*)lockedResource_;
	m_ringBufferLock = true;

	return true;
}

void VertexBuffer::NewFrame()
{
    m_ringLockedOffset = m_vertexRingOffset = m_ringLockedSize = 0;
    currentIndex = 0;
}

} // namespace EffekseerRendererLLGI
