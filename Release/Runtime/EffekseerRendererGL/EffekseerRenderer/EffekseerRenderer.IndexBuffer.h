
#pragma once

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRenderer.RendererImplemented.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.IndexBufferBase.h"
#include "EffekseerRenderer.DeviceObject.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
class IndexBuffer
	: public DeviceObject, public IndexBufferBase
{
private:
	GLuint					m_buffer;

	IndexBuffer( RendererImplemented* renderer, GLuint buffer, int maxCount, bool isDynamic );

public:
	virtual ~IndexBuffer();

	static IndexBuffer* Create( RendererImplemented* renderer, int maxCount, bool isDynamic );

	GLuint GetInterface() { return m_buffer; }

public:	// デバイス復旧用
	virtual void OnLostDevice();
	virtual void OnResetDevice();

public:
	void Lock();
	void Unlock();
};

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
}
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------