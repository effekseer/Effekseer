﻿
#ifndef __EFFEKSEERRENDERER_VERTEXBUFFER_BASE_H__
#define __EFFEKSEERRENDERER_VERTEXBUFFER_BASE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <Effekseer.h>
#include <assert.h>
#include <string.h>

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
class VertexBufferBase
{
protected:
	bool m_isDynamic;
	int m_size;
	int m_offset;
	uint8_t* m_resource;
	bool m_isLock;
	int m_vertexRingOffset = 0;
	bool isRingEnabled_ = true;

public:
	VertexBufferBase(int size, bool isDynamic);
	virtual ~VertexBufferBase();

	virtual void Lock() = 0;
	virtual bool RingBufferLock(int32_t size, int32_t& offset, void*& data, int32_t alignment) = 0;

	/**
		@brief	try lock as a ring buffer. if failed, it does't lock it.
	*/
	virtual bool TryRingBufferLock(int32_t size, int32_t& offset, void*& data, int32_t alignment) = 0;

	virtual void Unlock() = 0;
	virtual void Push(const void* buffer, int size);
	virtual int GetMaxSize() const;
	virtual int GetSize() const;
	virtual void* GetBufferDirect(int size);

	int GetVertexRingOffset() const
	{
		return m_vertexRingOffset;
	}

	bool GetIsRingEnabled() const
	{
		return isRingEnabled_;
	}

	static int GetNextAliginedVertexRingOffset(int vertexRingOffset, int alignment)
	{
		return (vertexRingOffset + alignment - 1) / alignment * alignment;
	}

	static int RequireResetRing(int vertexRingOffset, int size, int maxSize)
	{
		return (int32_t)vertexRingOffset + size > maxSize;
	}
};

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
} // namespace EffekseerRenderer
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_VERTEXBUFFER_BASE_H__