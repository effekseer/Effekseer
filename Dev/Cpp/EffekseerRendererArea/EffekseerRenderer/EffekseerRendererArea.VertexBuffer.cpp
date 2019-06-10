#include "EffekseerRendererArea.VertexBuffer.h"

namespace EffekseerRendererArea
{
VertexBuffer::VertexBuffer(int size, bool isDynamic) : EffekseerRenderer::VertexBufferBase(size, isDynamic)
{
	m_resource = new uint8_t[m_size];
	memset(m_resource, 0, (size_t)m_size);
	m_vertexRingOffset = 0;
}

VertexBuffer::~VertexBuffer() { delete[] m_resource; }

void VertexBuffer::Lock()
{
	assert(!m_isLock);

	m_isLock = true;
	m_offset = 0;
	m_vertexRingStart = 0;
}

bool VertexBuffer::RingBufferLock(int32_t size, int32_t& offset, void*& data)
{
	assert(!m_isLock);
	assert(!m_ringBufferLock);
	assert(this->m_isDynamic);

	if (size > m_size)
		return false;

	if ((int32_t)m_vertexRingOffset + size > m_size)
	{
		offset = 0;
		data = m_resource;
		m_vertexRingOffset = size;

		m_vertexRingStart = offset;
		m_offset = size;
	}
	else
	{
		offset = m_vertexRingOffset;
		data = m_resource + offset;
		m_vertexRingOffset += size;

		m_vertexRingStart = offset;
		m_offset = size;
	}

	m_ringBufferLock = true;

	return true;
}

bool VertexBuffer::TryRingBufferLock(int32_t size, int32_t& offset, void*& data)
{
	if ((int32_t)m_vertexRingOffset + size > m_size)
		return false;

	return RingBufferLock(size, offset, data);
}

void VertexBuffer::Unlock()
{
	assert(m_isLock || m_ringBufferLock);

	m_isLock = false;
	m_ringBufferLock = false;
}
} // namespace EffekseerRendererArea
