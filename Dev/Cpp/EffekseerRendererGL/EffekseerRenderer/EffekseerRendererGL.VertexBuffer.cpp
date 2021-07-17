
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
#include "EffekseerRendererGL.VertexBuffer.h"
#include "EffekseerRendererGL.GLExtension.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererGL
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
VertexBuffer::VertexBuffer(const Backend::GraphicsDeviceRef& graphicsDevice, bool isRingEnabled, int size, bool isDynamic, std::shared_ptr<SharedVertexTempStorage> storage)
	: DeviceObject(graphicsDevice.Get())
	, VertexBufferBase(size, isDynamic)
	, m_vertexRingStart(0)
	, m_ringBufferLock(false)
	, storage_(storage)
{
	isRingEnabled_ = isRingEnabled;

	if (storage == nullptr)
	{
		storage_ = std::make_shared<SharedVertexTempStorage>();
		storage_->buffer.resize(size);
	}
	else
	{
		assert(storage_->buffer.size() == size);
	}

	m_resource = nullptr;

	GLExt::glGenBuffers(1, &m_buffer);
	GLExt::glBindBuffer(GL_ARRAY_BUFFER, m_buffer);

	if (isRingEnabled_)
	{
		GLExt::glBufferData(GL_ARRAY_BUFFER, m_size, storage_->buffer.data(), GL_STREAM_DRAW);
	}

	GLExt::glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
VertexBuffer::~VertexBuffer()
{
	GLExt::glDeleteBuffers(1, &m_buffer);
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
VertexBuffer* VertexBuffer::Create(const Backend::GraphicsDeviceRef& graphicsDevice, bool isRingEnabled, int size, bool isDynamic, std::shared_ptr<SharedVertexTempStorage> storage)
{
	return new VertexBuffer(graphicsDevice, isRingEnabled, size, isDynamic, storage);
}

GLuint VertexBuffer::GetInterface()
{
	return m_buffer;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void VertexBuffer::OnLostDevice()
{
	GLExt::glDeleteBuffers(1, &m_buffer);
	m_buffer = 0;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void VertexBuffer::OnResetDevice()
{
	if (IsValid())
		return;

	GLExt::glGenBuffers(1, &m_buffer);
	GLExt::glBindBuffer(GL_ARRAY_BUFFER, m_buffer);

#ifndef __ANDROID__
	GLExt::glBufferData(GL_ARRAY_BUFFER, m_size, storage_->buffer.data(), GL_STREAM_DRAW);
#endif // !__ANDROID__

	GLExt::glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void VertexBuffer::Lock()
{
	assert(!m_isLock);
	m_isLock = true;
	m_offset = 0;
	m_vertexRingStart = 0;
	m_resource = storage_->buffer.data();
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
bool VertexBuffer::RingBufferLock(int32_t size, int32_t& offset, void*& data, int32_t alignment)
{
	assert(!m_isLock);
	assert(!m_ringBufferLock);
	assert(this->m_isDynamic);

	if (size > m_size)
		return false;

	m_vertexRingOffset = GetNextAliginedVertexRingOffset(m_vertexRingOffset, alignment);
	m_resource = storage_->buffer.data();

	if (RequireResetRing(m_vertexRingOffset, size, m_size) || !isRingEnabled_)
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
		data = m_resource;
		m_vertexRingOffset += size;

		m_vertexRingStart = offset;
		m_offset = size;
	}

	m_ringBufferLock = true;

	return true;
}

bool VertexBuffer::TryRingBufferLock(int32_t size, int32_t& offset, void*& data, int32_t alignment)
{
	if ((int32_t)m_vertexRingOffset + size > m_size)
		return false;

	return RingBufferLock(size, offset, data, alignment);
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void VertexBuffer::Unlock()
{
	assert(m_isLock || m_ringBufferLock);

	GLExt::glBindBuffer(GL_ARRAY_BUFFER, m_buffer);

	if (GLExt::IsSupportedBufferRange() && m_vertexRingStart > 0)
	{
		if (!isRingEnabled_)
		{
			GLExt::glBufferData(GL_ARRAY_BUFFER, m_offset, m_resource, GL_STREAM_DRAW);
		}

		auto target = GLExt::glMapBufferRange(GL_ARRAY_BUFFER, m_vertexRingStart, m_offset, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
		memcpy(target, m_resource, m_offset);
		GLExt::glUnmapBuffer(GL_ARRAY_BUFFER);
	}
	else
	{
		// giMapBuffer is invalid with OpenGLES3 after iOS12.2?
		bool avoidIOS122 = false;
#if defined(__APPLE__)
		if (GLExt::GetDeviceType() == OpenGLDeviceType::OpenGLES3)
		{
			avoidIOS122 = true;
		}
#endif

		if (GLExt::IsSupportedMapBuffer() && !avoidIOS122)
		{
			if (isRingEnabled_)
			{
				GLExt::glBufferData(GL_ARRAY_BUFFER, m_size, nullptr, GL_STREAM_DRAW);
			}
			else
			{
				GLExt::glBufferData(GL_ARRAY_BUFFER, m_offset, nullptr, GL_STREAM_DRAW);
			}

			auto target = (uint8_t*)GLExt::glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
			if (target == nullptr)
			{
				GLExt::MakeMapBufferInvalid();

				if (m_vertexRingStart > 0)
				{
					GLExt::glBufferSubData(GL_ARRAY_BUFFER, m_vertexRingStart, m_offset, m_resource);
				}
				else
				{
					GLExt::glBufferData(GL_ARRAY_BUFFER, m_size, m_resource, GL_STREAM_DRAW);
				}
			}
			else
			{
				memcpy(target + m_vertexRingStart, m_resource, m_offset);
				GLExt::glUnmapBuffer(GL_ARRAY_BUFFER);
			}
		}
		else
		{
			if (isRingEnabled_)
			{
				if (m_vertexRingStart > 0)
				{
					GLExt::glBufferSubData(GL_ARRAY_BUFFER, m_vertexRingStart, m_offset, m_resource);
				}
				else
				{
					GLExt::glBufferData(GL_ARRAY_BUFFER, m_size, m_resource, GL_STREAM_DRAW);
				}
			}
			else
			{
				GLExt::glBufferData(GL_ARRAY_BUFFER, m_offset, m_resource, GL_STREAM_DRAW);
			}
		}
	}

	GLExt::glBindBuffer(GL_ARRAY_BUFFER, 0);

	if (m_isLock)
	{
		m_vertexRingOffset += m_offset;
	}

	m_isLock = false;
	m_ringBufferLock = false;
	m_resource = nullptr;
}

bool VertexBuffer::IsValid()
{
	return m_buffer != 0;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
} // namespace EffekseerRendererGL
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
