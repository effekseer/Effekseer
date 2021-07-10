﻿
#include "EffekseerRendererLLGI.VertexBuffer.h"

namespace EffekseerRendererLLGI
{

VertexBuffer::VertexBuffer(Backend::GraphicsDevice* graphicsDevice, LLGI::VertexBuffer* buffer, int size, bool isDynamic, bool hasRefCount)
	: DeviceObject(graphicsDevice, hasRefCount)
	, VertexBufferBase(size, isDynamic)
	, m_ringBufferLock(false)
	, m_ringLockedOffset(0)
	, m_ringLockedSize(0)
{
	lockedResource_.resize(size);
	vertexBuffers.push_back(buffer);
}

VertexBuffer::~VertexBuffer()
{
	for (auto& v : vertexBuffers)
	{
		LLGI::SafeRelease(v);
	}
	vertexBuffers.clear();
}

VertexBuffer* VertexBuffer::Create(Backend::GraphicsDevice* graphicsDevice, int size, bool isDynamic, bool hasRefCount)
{
	auto vertexBuffer = graphicsDevice->GetGraphics()->CreateVertexBuffer(size);
	if (vertexBuffer == nullptr)
		return nullptr;

	return new VertexBuffer(graphicsDevice, vertexBuffer, size, isDynamic, hasRefCount);
}

void VertexBuffer::Lock()
{
	assert(!m_isLock);
	assert(!m_ringBufferLock);

	m_isLock = true;
	m_resource = (uint8_t*)lockedResource_.data();
	m_offset = 0;

	/* 次のRingBufferLockは強制的にDiscard */
	m_vertexRingOffset = m_size;
}

bool VertexBuffer::RingBufferLock(int32_t size, int32_t& offset, void*& data, int32_t alignment)
{
	assert(!m_isLock);
	assert(!m_ringBufferLock);
	assert(this->m_isDynamic);

	if (size > m_size)
		return false;

	m_vertexRingOffset = GetNextAliginedVertexRingOffset(m_vertexRingOffset, alignment);

	if (RequireResetRing(m_vertexRingOffset, size, m_size))
	{
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

	data = (uint8_t*)lockedResource_.data();
	m_resource = (uint8_t*)lockedResource_.data();
	m_ringBufferLock = true;

	return true;
}

bool VertexBuffer::TryRingBufferLock(int32_t size, int32_t& offset, void*& data, int32_t alignment)
{
	if ((int32_t)m_vertexRingOffset + size > m_size)
		return false;

	return RingBufferLock(size, offset, data, alignment);
}

void VertexBuffer::Unlock()
{
	assert(m_isLock || m_ringBufferLock);

	if (m_isLock)
	{
		auto data = (uint8_t*)vertexBuffers[currentIndex]->Lock();

		memcpy(data, m_resource, m_size);

		vertexBuffers[currentIndex]->Unlock();
	}

	if (m_ringBufferLock)
	{
		auto data = (uint8_t*)vertexBuffers[currentIndex]->Lock();

		uint8_t* dst = (uint8_t*)data;
		dst += m_ringLockedOffset;

		uint8_t* src = (uint8_t*)m_resource;
		// src += m_ringLockedOffset;

		memcpy(dst, src, m_ringLockedSize);

		vertexBuffers[currentIndex]->Unlock();
	}

	m_resource = nullptr;
	m_isLock = false;
	m_ringBufferLock = false;
}

} // namespace EffekseerRendererLLGI
