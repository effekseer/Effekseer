
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
class IndexBuffer : public DeviceObject, public ::EffekseerRenderer::IndexBufferBase
{
private:
	GLuint m_buffer;

	IndexBuffer(const Backend::GraphicsDeviceRef& graphicsDevice, GLuint buffer, int maxCount, bool isDynamic, int32_t stride);

public:
	virtual ~IndexBuffer();

	static IndexBuffer* Create(const Backend::GraphicsDeviceRef& graphicsDevice, int maxCount, bool isDynamic, int32_t stride);

	GLuint GetInterface()
	{
		return m_buffer;
	}

public:
	void OnLostDevice() override;
	void OnResetDevice() override;

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
