
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
#include "EffekseerRendererDX11.IndexBuffer.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererDX11
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
IndexBuffer::IndexBuffer(RendererImplemented* renderer, ID3D11Buffer* buffer, int maxCount, bool isDynamic)
	: DeviceObject(renderer)
	, IndexBufferBase(maxCount, isDynamic)
	, m_buffer(buffer)
	, m_lockedResource(NULL)
{
	m_lockedResource = new uint8_t[sizeof(uint16_t) * maxCount];
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
IndexBuffer::~IndexBuffer()
{
	ES_SAFE_RELEASE(m_buffer);
	ES_SAFE_DELETE_ARRAY(m_lockedResource);
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
IndexBuffer* IndexBuffer::Create(RendererImplemented* renderer, int maxCount, bool isDynamic)
{
	D3D11_BUFFER_DESC hBufferDesc;
	hBufferDesc.Usage = isDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	hBufferDesc.ByteWidth = sizeof(uint16_t) * maxCount;
	hBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	hBufferDesc.CPUAccessFlags = isDynamic ? D3D11_CPU_ACCESS_WRITE : 0;
	hBufferDesc.MiscFlags = 0;
	hBufferDesc.StructureByteStride = sizeof(uint16_t);

	D3D11_SUBRESOURCE_DATA hSubResourceData;
	hSubResourceData.pSysMem = NULL;
	hSubResourceData.SysMemPitch = 0;
	hSubResourceData.SysMemSlicePitch = 0;

	// 生成
	ID3D11Buffer* ib = NULL;
	if (FAILED(renderer->GetDevice()->CreateBuffer(&hBufferDesc, NULL, &ib)))
	{
		return NULL;
	}

	return new IndexBuffer(renderer, ib, maxCount, isDynamic);
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void IndexBuffer::OnLostDevice()
{
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void IndexBuffer::OnResetDevice()
{
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void IndexBuffer::Lock()
{
	assert(!m_isLock);

	m_isLock = true;
	m_resource = (uint8_t*)m_lockedResource;
	m_indexCount = 0;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void IndexBuffer::Unlock()
{
	assert(m_isLock);

	if (m_isDynamic)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		GetRenderer()->GetContext()->Map(
			m_buffer,
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&mappedResource);

		memcpy(mappedResource.pData, m_resource, sizeof(uint16_t) * GetMaxCount());

		GetRenderer()->GetContext()->Unmap(m_buffer, 0);
	}
	else
	{
		GetRenderer()->GetContext()->UpdateSubresource(
			m_buffer,
			0,
			NULL,
			m_resource,
			0,
			0);
	}

	m_resource = NULL;
	m_isLock = false;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
} // namespace EffekseerRendererDX11
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
