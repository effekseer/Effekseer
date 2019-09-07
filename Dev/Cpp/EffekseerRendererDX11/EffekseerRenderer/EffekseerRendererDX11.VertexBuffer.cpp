
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
#include "EffekseerRendererDX11.VertexBuffer.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererDX11
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
VertexBuffer::VertexBuffer( RendererImplemented* renderer, ID3D11Buffer* buffer, int size, bool isDynamic )
	: DeviceObject			( renderer )
	, VertexBufferBase		( size, isDynamic )
	, m_buffer				( buffer )
	, m_vertexRingOffset	( 0 )
	, m_ringBufferLock		( false )
	, m_ringLockedOffset	( 0 )
	, m_ringLockedSize		( 0 )
{
	m_lockedResource = new uint8_t[ size ];
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
VertexBuffer::~VertexBuffer()
{
	ES_SAFE_DELETE_ARRAY( m_lockedResource );
	ES_SAFE_RELEASE( m_buffer );
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
VertexBuffer* VertexBuffer::Create( RendererImplemented* renderer, int size, bool isDynamic )
{
	D3D11_BUFFER_DESC hBufferDesc;
	ZeroMemory(&hBufferDesc, sizeof(hBufferDesc));

	hBufferDesc.ByteWidth = size;
	hBufferDesc.Usage = isDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	hBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	hBufferDesc.CPUAccessFlags = isDynamic ? D3D11_CPU_ACCESS_WRITE : 0;
	hBufferDesc.MiscFlags = 0;
	hBufferDesc.StructureByteStride = sizeof(float);

	D3D11_SUBRESOURCE_DATA hSubResourceData;
	hSubResourceData.pSysMem = NULL;
	hSubResourceData.SysMemPitch = 0;
	hSubResourceData.SysMemSlicePitch = 0;

	
	// 生成
	ID3D11Buffer* vb = NULL;
	HRESULT hr = renderer->GetDevice()->CreateBuffer(&hBufferDesc, NULL, &vb);
	if( FAILED( hr ) )
	{
		return NULL;
	}

	return new VertexBuffer( renderer, vb, size, isDynamic );
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void VertexBuffer::OnLostDevice()
{
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void VertexBuffer::OnResetDevice()
{
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void VertexBuffer::Lock()
{
	assert( !m_isLock );
	assert( !m_ringBufferLock );

	m_isLock = true;
	m_resource = (uint8_t*)m_lockedResource;
	m_offset = 0;


	/* 次のRingBufferLockは強制的にDiscard */
	m_vertexRingOffset = m_size;
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

	if ( (int32_t)m_vertexRingOffset + size > m_size )
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

	data = (uint8_t*)m_lockedResource;
	m_resource = (uint8_t*)m_lockedResource;
	m_ringBufferLock = true;

	return true;
}

bool VertexBuffer::TryRingBufferLock(int32_t size, int32_t& offset, void*& data)
{
	if ((int32_t)m_vertexRingOffset + size > m_size) return false;

	return RingBufferLock(size, offset, data);
}


//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void VertexBuffer::Unlock()
{
	assert( m_isLock || m_ringBufferLock );
	
	if( m_isLock )
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		GetRenderer()->GetContext()->Map(
			m_buffer,
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&mappedResource );
	
		memcpy( mappedResource.pData, m_resource, m_size );
	
		GetRenderer()->GetContext()->Unmap( m_buffer, 0 );
	}

	if(m_ringBufferLock)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		GetRenderer()->GetContext()->Map(
			m_buffer,
			0,
			m_ringLockedOffset != 0 ? D3D11_MAP_WRITE_NO_OVERWRITE : D3D11_MAP_WRITE_DISCARD,
			0,
			&mappedResource );
	
		uint8_t* dst = (uint8_t*)mappedResource.pData;
		dst += m_ringLockedOffset;

		uint8_t* src = (uint8_t*) m_resource;
		//src += m_ringLockedOffset;

		memcpy( dst, src, m_ringLockedSize );
	
		GetRenderer()->GetContext()->Unmap( m_buffer, 0 );
	}

	m_resource = NULL;
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