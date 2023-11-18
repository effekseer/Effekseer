#ifndef __EFFEKSEERRENDERER_VERTEXBUFFER_H__
#define __EFFEKSEERRENDERER_VERTEXBUFFER_H__

#include <Effekseer.h>
#include <memory>
#include <stdint.h>
#include <vector>

namespace EffekseerRenderer
{

class VertexBuffer
{
protected:
	int32_t size_ = 0;

public:
	virtual ~VertexBuffer() = default;
	virtual bool GetIsValid() const = 0;
	virtual bool CanAllocate(int32_t size, int32_t alignment) = 0;
	virtual bool Allocate(int32_t size, int32_t alignment, std::tuple<void*, int32_t>& result) = 0;
	virtual Effekseer::Backend::VertexBufferRef Upload() = 0;
	virtual Effekseer::Backend::VertexBufferRef GetCurrentBuffer() = 0;

	virtual void RenewBuffer() = 0;

	virtual int32_t GetSize() const
	{
		return size_;
	}

	static int GetNextAliginedOffset(int32_t offset, int32_t alignment)
	{
		return (offset + alignment - 1) / alignment * alignment;
	}
};

class VertexBufferMultiSize : public VertexBuffer
{
	int32_t offset_ = 0;
	Effekseer::CustomAlignedVector<uint8_t> buffer_;
	std::vector<Effekseer::Backend::VertexBufferRef> vertexBuffers_;

public:
	VertexBufferMultiSize(Effekseer::Backend::GraphicsDeviceRef graphicsDevice, int32_t size);

	bool GetIsValid() const override;

	bool CanAllocate(int32_t size, int32_t alignment) override;

	bool Allocate(int32_t size, int32_t alignment, std::tuple<void*, int32_t>& result) override;

	Effekseer::Backend::VertexBufferRef Upload() override;

	Effekseer::Backend::VertexBufferRef GetCurrentBuffer() override;

	void RenewBuffer() override;
};

class VertexBufferRing : public VertexBuffer
{
	int currentIndex_ = 0;
	int offset_ = 0;
	int previous_offset_ = 0;
	Effekseer::CustomAlignedVector<uint8_t> buffer_;
	std::vector<Effekseer::Backend::VertexBufferRef> vertexBuffers_;

public:
	VertexBufferRing(Effekseer::Backend::GraphicsDeviceRef graphicsDevice, int32_t size, int32_t ringCount);

	bool GetIsValid() const override;

	bool CanAllocate(int32_t size, int32_t alignment) override;

	bool Allocate(int32_t size, int32_t alignment, std::tuple<void*, int32_t>& result) override;

	Effekseer::Backend::VertexBufferRef Upload() override;

	Effekseer::Backend::VertexBufferRef GetCurrentBuffer() override;

	void RenewBuffer() override;
};

} // namespace EffekseerRenderer

#endif