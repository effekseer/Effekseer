
#pragma once

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "../../EffekseerRendererCommon/EffekseerRenderer.IndexBufferBase.h"
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
class IndexBuffer : public DeviceObject, public ::EffekseerRenderer::IndexBufferBase
{
private:
	ID3D11Buffer* m_buffer;
	void* m_lockedResource;

	IndexBuffer(RendererImplemented* renderer, ID3D11Buffer* buffer, int maxCount, bool isDynamic, bool hasRefCount);

public:
	virtual ~IndexBuffer();

	static IndexBuffer* Create(RendererImplemented* renderer, int maxCount, bool isDynamic, bool hasRefCount);

	ID3D11Buffer* GetInterface()
	{
		return m_buffer;
	}

public: // デバイス復旧用
	virtual void OnLostDevice();
	virtual void OnResetDevice();

public:
	void Lock();
	void Unlock();
};

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
} // namespace EffekseerRendererDX11
  //-----------------------------------------------------------------------------------
  //
  //-----------------------------------------------------------------------------------