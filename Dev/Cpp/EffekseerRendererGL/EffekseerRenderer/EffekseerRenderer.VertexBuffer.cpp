
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
#include "EffekseerRenderer.VertexBuffer.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
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

	glGenBuffers(1, &m_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
	glBufferData(GL_ARRAY_BUFFER, m_size, m_resource, GL_STREAM_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1, &m_buffer);
	delete [] m_resource;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
VertexBuffer* VertexBuffer::Create( RendererImplemented* renderer, int size, bool isDynamic )
{
	return new VertexBuffer( renderer, size, isDynamic );
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void VertexBuffer::OnLostDevice()
{
	glDeleteBuffers(1, &m_buffer);
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void VertexBuffer::OnResetDevice()
{
	glGenBuffers(1, &m_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
	glBufferData(GL_ARRAY_BUFFER, m_size, m_resource, GL_STREAM_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
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

	//if ( m_vertexRingOffset + size > m_size )
	if( true )
	{
		offset = 0;
		data = m_resource;
		m_vertexRingOffset = size;
		m_offset = m_vertexRingOffset - offset;

		m_vertexRingStart = offset;
	}
	else
	{
		offset = m_vertexRingOffset;
		data = m_resource;
		m_vertexRingOffset += size;

		m_vertexRingStart = offset;
		m_offset = m_vertexRingOffset - offset;
	}

	m_ringBufferLock = true;

	return true;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void VertexBuffer::Unlock()
{
	assert( m_isLock || m_ringBufferLock );

	glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
	glBufferSubData(GL_ARRAY_BUFFER, m_vertexRingStart, m_offset, m_resource);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	m_isLock = false;
	m_ringBufferLock = false;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
}
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------