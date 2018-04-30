
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
VertexBuffer::VertexBuffer( RendererImplemented* renderer, int size, bool isDynamic )
	: DeviceObject		( renderer )
	, VertexBufferBase	( size, isDynamic )
	, m_vertexRingStart		( 0 )
	, m_vertexRingOffset	( 0 )
	, m_ringBufferLock		( false )
{
	m_resource = new uint8_t[m_size];
	memset(m_resource, 0, (size_t)m_size);

	// it is for android opengl ES2.0 mainly
	nBufferingMode = !GLExt::IsSupportedBufferRange() && GLExt::IsSupportedMapBuffer();

	if (nBufferingMode)
	{
		bufferingCount = 3;
		GLExt::glGenBuffers(bufferingCount, m_buffers);

		for (int32_t i = 0; i < bufferingCount; i++)
		{
			GLExt::glBindBuffer(GL_ARRAY_BUFFER, m_buffers[i]);
			GLExt::glBufferData(GL_ARRAY_BUFFER, m_size, m_resource, GL_STREAM_DRAW);
			GLExt::glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
	}
	else
	{
		GLExt::glGenBuffers(1, &m_buffers[0]);
		GLExt::glBindBuffer(GL_ARRAY_BUFFER, m_buffers[0]);
		GLExt::glBufferData(GL_ARRAY_BUFFER, m_size, m_resource, GL_STREAM_DRAW);
		GLExt::glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
VertexBuffer::~VertexBuffer()
{
	if (nBufferingMode)
	{
		GLExt::glDeleteBuffers(bufferingCount, m_buffers);
	}
	else
	{
		GLExt::glDeleteBuffers(1, &m_buffers[0]);
	}

	delete [] m_resource;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
VertexBuffer* VertexBuffer::Create( RendererImplemented* renderer, int size, bool isDynamic )
{
	return new VertexBuffer( renderer, size, isDynamic );
}

GLuint VertexBuffer::GetInterface()
{
	return m_buffers[bufferingIndex];
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void VertexBuffer::OnLostDevice()
{
	if (nBufferingMode)
	{
		GLExt::glDeleteBuffers(bufferingCount, m_buffers);

		for (int32_t i = 0; i < bufferingCount; i++)
		{
			m_buffers[i] = 0;
		}
	}
	else
	{
		GLExt::glDeleteBuffers(1, &m_buffers[0]);
		m_buffers[0] = 0;
	}
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void VertexBuffer::OnResetDevice()
{
	if (IsValid()) return;

	if (nBufferingMode)
	{
		bufferingCount = 3;
		GLExt::glGenBuffers(bufferingCount, m_buffers);

		for (int32_t i = 0; i < bufferingCount; i++)
		{
			GLExt::glBindBuffer(GL_ARRAY_BUFFER, m_buffers[i]);
			GLExt::glBufferData(GL_ARRAY_BUFFER, m_size, m_resource, GL_STREAM_DRAW);
			GLExt::glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
	}
	else
	{
		GLExt::glGenBuffers(1, &m_buffers[0]);
		GLExt::glBindBuffer(GL_ARRAY_BUFFER, m_buffers[0]);
		GLExt::glBufferData(GL_ARRAY_BUFFER, m_size, m_resource, GL_STREAM_DRAW);
		GLExt::glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void VertexBuffer::Lock()
{
	assert( !m_isLock );

	m_isLock = true;
	m_offset = 0;
	m_vertexRingStart = 0;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
bool VertexBuffer::RingBufferLock( int32_t size, int32_t& offset, void*& data )
{
	assert( !m_isLock );
	assert( !m_ringBufferLock );
	assert( this->m_isDynamic );

	if( size > m_size ) return false;

	// glDrawElementsでオフセットを指定できないため
	if ( m_vertexRingOffset + size > m_size )
	//if( true )
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

bool VertexBuffer::TryRingBufferLock(int32_t size, int32_t& offset, void*& data)
{
	if ((int32_t) m_vertexRingOffset + size > m_size) return false;

	return RingBufferLock(size, offset, data);
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void VertexBuffer::Unlock()
{
	assert( m_isLock || m_ringBufferLock );

	if (nBufferingMode)
	{
		bufferingIndex++;
		bufferingIndex = bufferingIndex % bufferingCount;
	}

	GLExt::glBindBuffer(GL_ARRAY_BUFFER, m_buffers[bufferingIndex]);


	if (GLExt::IsSupportedBufferRange() && m_vertexRingOffset > 0)
	{
		auto target = GLExt::glMapBufferRange(GL_ARRAY_BUFFER, m_vertexRingStart, m_offset, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
		memcpy(target, m_resource, m_offset);
		GLExt::glUnmapBuffer(GL_ARRAY_BUFFER);
	}
	else
	{
		if (nBufferingMode)
		{
			auto target = (uint8_t*)GLExt::glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
			memcpy(target + m_vertexRingStart, m_resource, m_offset);
			GLExt::glUnmapBuffer(GL_ARRAY_BUFFER);
		}
		else
		{
			GLExt::glBufferSubData(GL_ARRAY_BUFFER, m_vertexRingStart, m_offset, m_resource);
		}
	}

	GLExt::glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	m_isLock = false;
	m_ringBufferLock = false;
}

bool VertexBuffer::IsValid()
{
	if (nBufferingMode)
	{
		return m_buffers[bufferingIndex] != 0;
	}
	else
	{
		return m_buffers[0] != 0;
	}
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
}
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------