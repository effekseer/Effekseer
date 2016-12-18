
#pragma once

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererDX9.RendererImplemented.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.VertexBufferBase.h"
#include "EffekseerRendererDX9.DeviceObject.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererDX9
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
class VertexBuffer
	: public DeviceObject
	, public ::EffekseerRenderer::VertexBufferBase
{
private:
	IDirect3DVertexBuffer9*	m_buffer;

	uint32_t				m_vertexRingOffset;
	bool					m_ringBufferLock;

	VertexBuffer( RendererImplemented* renderer, IDirect3DVertexBuffer9* buffer, int size, bool isDynamic );
public:
	virtual ~VertexBuffer();

	static VertexBuffer* Create( RendererImplemented* renderer, int size, bool isDynamic );

	IDirect3DVertexBuffer9* GetInterface() { return m_buffer; }

public:	// デバイス復旧用
	virtual void OnLostDevice();
	virtual void OnChangeDevice();
	virtual void OnResetDevice();

public:
	void Lock();
	bool RingBufferLock( int32_t size, int32_t& offset, void*& data );
	bool TryRingBufferLock(int32_t size, int32_t& offset, void*& data);
	void Unlock();
};

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
}
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------