
#pragma once

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "../../EffekseerRendererCommon/EffekseerRenderer.IndexBufferBase.h"
#include "EffekseerRendererDX9.DeviceObject.h"
#include "EffekseerRendererDX9.RendererImplemented.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererDX9
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
class IndexBuffer : public DeviceObject, public ::EffekseerRenderer::IndexBufferBase
{
private:
	IDirect3DIndexBuffer9* m_buffer;

	IndexBuffer(RendererImplemented* renderer, IDirect3DIndexBuffer9* buffer, int maxCount, bool isDynamic, bool hasRefCount);

public:
	virtual ~IndexBuffer();

	static IndexBuffer* Create(RendererImplemented* renderer, int maxCount, bool isDynamic, bool hasRefCount);

	IDirect3DIndexBuffer9* GetInterface()
	{
		return m_buffer;
	}

public: // デバイス復旧用
	virtual void OnLostDevice();
	virtual void OnChangeDevice();
	virtual void OnResetDevice();

public:
	void Lock();
	void Unlock();
};

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
} // namespace EffekseerRendererDX9
  //-----------------------------------------------------------------------------------
  //
  //-----------------------------------------------------------------------------------