
#pragma once

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "../../EffekseerRendererCommon/EffekseerRenderer.VertexBufferBase.h"
#include "EffekseerRendererDX11.DeviceObject.h"
#include "EffekseerRendererDX11.RendererImplemented.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererDX11
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
class VertexBuffer
	: public DeviceObject,
	  public ::EffekseerRenderer::VertexBufferBase
{
private:
	ID3D11Buffer* m_buffer;
	void* m_lockedResource;

	uint32_t m_vertexRingOffset;
	bool m_ringBufferLock;

	int32_t m_ringLockedOffset;
	int32_t m_ringLockedSize;

	VertexBuffer(RendererImplemented* renderer, ID3D11Buffer* buffer, int size, bool isDynamic);

public:
	virtual ~VertexBuffer();

	static VertexBuffer* Create(RendererImplemented* renderer, int size, bool isDynamic);

	ID3D11Buffer* GetInterface()
	{
		return m_buffer;
	}

public: // デバイス復旧用
	virtual void OnLostDevice();
	virtual void OnResetDevice();

public:
	void Lock();
	bool RingBufferLock(int32_t size, int32_t& offset, void*& data, int32_t alignment) override;
	bool TryRingBufferLock(int32_t size, int32_t& offset, void*& data, int32_t alignment) override;
	void Unlock();
};

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
} // namespace EffekseerRendererDX11
  //-----------------------------------------------------------------------------------
  //
  //-----------------------------------------------------------------------------------