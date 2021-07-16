
#pragma once

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "../../EffekseerRendererCommon/EffekseerRenderer.VertexBufferBase.h"
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

struct SharedVertexTempStorage
{
	Effekseer::CustomAlignedVector<uint8_t> buffer;
};

class VertexBuffer : public DeviceObject, public ::EffekseerRenderer::VertexBufferBase
{
private:
	GLuint m_buffer;

	std::shared_ptr<SharedVertexTempStorage> storage_;
	uint32_t m_vertexRingStart;
	bool m_ringBufferLock;

	VertexBuffer(const Backend::GraphicsDeviceRef& graphicsDevice, bool isRingEnabled, int size, bool isDynamic, std::shared_ptr<SharedVertexTempStorage> storage = nullptr);

public:
	virtual ~VertexBuffer();

	static VertexBuffer* Create(const Backend::GraphicsDeviceRef& graphicsDevice, bool isRingEnabled, int size, bool isDynamic, std::shared_ptr<SharedVertexTempStorage> storage = nullptr);

	GLuint GetInterface();

public:
	void OnLostDevice() override;
	void OnResetDevice() override;

public:
	void Lock();
	bool RingBufferLock(int32_t size, int32_t& offset, void*& data, int32_t alignment) override;
	bool TryRingBufferLock(int32_t size, int32_t& offset, void*& data, int32_t alignment) override;

	void Unlock();

	bool IsValid();
};

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
} // namespace EffekseerRendererGL
  //-----------------------------------------------------------------------------------
  //
  //-----------------------------------------------------------------------------------