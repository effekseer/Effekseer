
#pragma once

#include "../../EffekseerRendererLLGI/EffekseerRendererLLGI.VertexBuffer.h"

namespace EffekseerRendererMetal
{

class VertexBuffer : public ::EffekseerRendererLLGI::VertexBuffer
{
private:

	VertexBuffer(::EffekseerRendererLLGI::GraphicsDevice* graphicsDevice, LLGI::VertexBuffer* buffer, int size, bool isDynamic, bool hasRefCount);
    
    void AddNewBuffer();

public:

	static VertexBuffer* Create(::EffekseerRendererLLGI::GraphicsDevice* graphicsDevice, int size, bool isDynamic, bool hasRefCount);

public:
	bool RingBufferLock(int32_t size, int32_t& offset, void*& data, int32_t alignment) override;
    void NewFrame();
};

} // namespace EffekseerRendererLLGI
