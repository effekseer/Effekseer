
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
#include "EffekseerRendererDX9.IndexBuffer.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererDX9
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
IndexBuffer::IndexBuffer( RendererImplemented* renderer, IDirect3DIndexBuffer9* buffer, int maxCount, bool isDynamic )
	: DeviceObject		( renderer )
	, IndexBufferBase	( maxCount, isDynamic )
	, m_buffer			( buffer )
{
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
IndexBuffer::~IndexBuffer()
{
	ES_SAFE_RELEASE( m_buffer );
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
IndexBuffer* IndexBuffer::Create( RendererImplemented* renderer, int maxCount, bool isDynamic )
{
	// フラグ
	D3DPOOL pool = D3DPOOL_MANAGED;
	int usage = D3DUSAGE_WRITEONLY;

	if ( isDynamic )
	{
		usage |= D3DUSAGE_DYNAMIC;
		pool = D3DPOOL_DEFAULT;
	}

	// フォーマット
	D3DFORMAT format = D3DFMT_INDEX16;
	int size = sizeof(uint16_t);

	// 生成
	IDirect3DIndexBuffer9* ib = NULL;
	HRESULT hr = renderer->GetDevice()->CreateIndexBuffer( maxCount * size, usage, format, pool, &ib, NULL );
	
	if( FAILED( hr ) )
	{
		// DirectX9ExではD3DPOOL_MANAGED使用不可
		pool = D3DPOOL_DEFAULT;
		hr = renderer->GetDevice()->CreateIndexBuffer( maxCount * size, usage, format, pool, &ib, NULL );
	}

	if( FAILED( hr ) ) return NULL;

	return new IndexBuffer( renderer, ib, maxCount, isDynamic );
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void IndexBuffer::OnLostDevice()
{
	if ( m_isDynamic )
	{
		ES_SAFE_RELEASE( m_buffer );
	}
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void IndexBuffer::OnChangeDevice()
{
	ES_SAFE_RELEASE( m_buffer );
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void IndexBuffer::OnResetDevice()
{
	// バッファ生成
	if ( m_buffer == NULL )
	{
		// フラグ
		D3DPOOL pool = D3DPOOL_MANAGED;
		int usage = D3DUSAGE_WRITEONLY;

		if ( m_isDynamic )
		{
			usage |= D3DUSAGE_DYNAMIC;
			pool = D3DPOOL_DEFAULT;
		}

		// フォーマット
		D3DFORMAT format = D3DFMT_INDEX16;
		int size = sizeof(uint16_t);

		HRESULT hr = GetRenderer()->GetDevice()->CreateIndexBuffer( m_indexMaxCount * size, usage, format, pool, &m_buffer, NULL );
	
		if( FAILED( hr ) )
		{
			// DirectX9ExではD3DPOOL_MANAGED使用不可
			pool = D3DPOOL_DEFAULT;
			hr = GetRenderer()->GetDevice()->CreateIndexBuffer( m_indexMaxCount * size, usage, format, pool, &m_buffer, NULL );
		}
	}
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void IndexBuffer::Lock()
{
	assert( !m_isLock );

	m_isLock = true;
	m_resource = NULL;
	m_indexCount = 0;

	m_buffer->Lock( 0, 0, (void**)&m_resource, m_isDynamic ? D3DLOCK_DISCARD : 0 );
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void IndexBuffer::Unlock()
{
	assert( m_isLock );

	
	if ( m_resource != NULL )
	{
		m_resource = NULL;
		m_buffer->Unlock();
	}
	
	m_isLock = false;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
}
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
