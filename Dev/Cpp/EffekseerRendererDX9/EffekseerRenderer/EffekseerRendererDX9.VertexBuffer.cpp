
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
#include "EffekseerRendererDX9.VertexBuffer.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererDX9
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
VertexBuffer::VertexBuffer(RendererImplemented* renderer, IDirect3DVertexBuffer9* buffer, int size, bool isDynamic)
	: DeviceObject(renderer)
	, VertexBufferBase(size, isDynamic)
	, m_buffer(buffer)
	, m_vertexRingOffset(0)
	, m_ringBufferLock(false)
{
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
VertexBuffer::~VertexBuffer()
{
	ES_SAFE_RELEASE(m_buffer);
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
VertexBuffer* VertexBuffer::Create(RendererImplemented* renderer, int size, bool isDynamic)
{
	// フラグ
	D3DPOOL pool = D3DPOOL_MANAGED;
	int usage = D3DUSAGE_WRITEONLY;

	if (isDynamic)
	{
		pool = D3DPOOL_DEFAULT;
		usage |= D3DUSAGE_DYNAMIC;
	}

	IDirect3DVertexBuffer9* vb = NULL;
	HRESULT hr = renderer->GetDevice()->CreateVertexBuffer(size, usage, 0, pool, &vb, NULL);

	if (FAILED(hr))
	{
		// DirectX9ExではD3DPOOL_MANAGED使用不可
		pool = D3DPOOL_DEFAULT;
		hr = renderer->GetDevice()->CreateVertexBuffer(size, usage, 0, pool, &vb, NULL);
	}

	if (FAILED(hr))
		return NULL;

	return new VertexBuffer(renderer, vb, size, isDynamic);
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void VertexBuffer::OnLostDevice()
{
	if (m_isDynamic)
	{
		ES_SAFE_RELEASE(m_buffer);
	}
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void VertexBuffer::OnChangeDevice()
{
	ES_SAFE_RELEASE(m_buffer);
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void VertexBuffer::OnResetDevice()
{
	// バッファ生成
	if (m_buffer == NULL)
	{
		D3DPOOL pool = D3DPOOL_MANAGED;
		int usage = D3DUSAGE_WRITEONLY;

		if (m_isDynamic)
		{
			pool = D3DPOOL_DEFAULT;
			usage |= D3DUSAGE_DYNAMIC;
		}

		LPDIRECT3DDEVICE9 device = GetRenderer()->GetDevice();
		assert(device != NULL);

		HRESULT hr = device->CreateVertexBuffer(m_size, usage, 0, pool, &m_buffer, NULL);

		if (FAILED(hr))
		{
			// DirectX9ExではD3DPOOL_MANAGED使用不可
			pool = D3DPOOL_DEFAULT;
			hr = device->CreateVertexBuffer(m_size, usage, 0, pool, &m_buffer, NULL);
		}
	}
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void VertexBuffer::Lock()
{
	assert(!m_isLock);
	assert(!m_ringBufferLock);

	m_isLock = true;
	m_resource = NULL;
	m_offset = 0;

	m_buffer->Lock(0, 0, (void**)&m_resource, m_isDynamic ? D3DLOCK_DISCARD : 0);

	/* 次のRingBufferLockは強制的にDiscard */
	m_vertexRingOffset = m_size;
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

	m_vertexRingOffset = (m_vertexRingOffset + alignment - 1) / alignment * alignment;

	if ((int32_t)m_vertexRingOffset + size > m_size)
	{
		offset = 0;
		m_buffer->Lock(0, 0, reinterpret_cast<void**>(&data), D3DLOCK_DISCARD);
		m_vertexRingOffset = (uint32_t)size;
	}
	else
	{
		offset = m_vertexRingOffset;
		m_buffer->Lock(offset, size, reinterpret_cast<void**>(&data), D3DLOCK_NOOVERWRITE);
		m_vertexRingOffset += size;
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

	m_resource = NULL;
	m_buffer->Unlock();

	m_isLock = false;
	m_ringBufferLock = false;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
} // namespace EffekseerRendererDX9
  //-----------------------------------------------------------------------------------
  //
  //-----------------------------------------------------------------------------------