
#pragma once

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "../../EffekseerRendererCommon/EffekseerRenderer.IndexBufferBase.h"
#include "EffekseerRendererGL.DeviceObject.h"
#include "EffekseerRendererGL.RendererImplemented.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererGL
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
class IndexBuffer
	: public DeviceObject,
	  public ::EffekseerRenderer::IndexBufferBase
{
private:
	GLuint m_buffer;

	IndexBuffer(RendererImplemented* renderer, GLuint buffer, int maxCount, bool isDynamic, int32_t stride, bool hasRefCount);

public:
	virtual ~IndexBuffer();

	static IndexBuffer* Create(RendererImplemented* renderer, int maxCount, bool isDynamic, int32_t stride, bool hasRefCount);

	GLuint GetInterface()
	{
		return m_buffer;
	}

public: // デバイス復旧用
	virtual void OnLostDevice() override;
	virtual void OnResetDevice() override;

public:
	void Lock() override;
	void Unlock() override;

	bool IsValid();

	int32_t GetStride() const
	{
		return stride_;
	}
};

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
} // namespace EffekseerRendererGL
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
