#ifndef __EFFEKSEERRENDERER_VERTEXBUFFER_H__
#define __EFFEKSEERRENDERER_VERTEXBUFFER_H__

#include <Effekseer.h>
#include <memory>
#include <vector>

namespace EffekseerRenderer
{

class VertexBuffer
{
protected:
	int32_t size_ = 0;

public:
	virtual ~VertexBuffer() = default;

	virtual bool CanAllocate(int32_t size, int32_t alignment) = 0;
	virtual bool Allocate(int32_t size, int32_t alignment, std::tuple<void*, int32_t>& result) = 0;
	virtual Effekseer::Backend::VertexBufferRef Upload() = 0;
	virtual Effekseer::Backend::VertexBufferRef GetCurrentBuffer() = 0;

	virtual bool UpdateData(const void* data, int32_t size, int32_t alignment, std::tuple<Effekseer::Backend::VertexBufferRef, int32_t>& result) = 0;

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
	VertexBufferMultiSize(Effekseer::Backend::GraphicsDeviceRef graphicsDevice, int32_t size)
	{
		size_ = size;
		buffer_.resize(size_);

		int current_size = 16;
		while (current_size < size_ * 2)
		{
			vertexBuffers_.emplace_back(graphicsDevice->CreateVertexBuffer(current_size, nullptr, true));
			current_size *= 2;
		}
	}

	bool CanAllocate(int32_t size, int32_t alignment) override
	{
		return size + GetNextAliginedOffset(offset_, alignment) <= size_;
	}

	bool Allocate(int32_t size, int32_t alignment, std::tuple<void*, int32_t>& result) override
	{
		if (!CanAllocate(size, alignment))
		{
			return false;
		}

		auto next_offset = GetNextAliginedOffset(offset_, alignment);
		offset_ = next_offset + size;
		result = {buffer_.data() + next_offset, next_offset};
		return true;
	}

	Effekseer::Backend::VertexBufferRef Upload() override
	{
		int current_size = 16;
		for (int i = 0;;)
		{
			if (offset_ <= current_size)
			{
				vertexBuffers_[i]->UpdateData(buffer_.data(), offset_, 0);
				return vertexBuffers_[i];
			}
			i++;
			current_size *= 2;
		}

		return nullptr;
	}

	Effekseer::Backend::VertexBufferRef GetCurrentBuffer() override
	{
		return nullptr;
	}

	void RenewBuffer() override
	{
		offset_ = 0;
	}

	bool UpdateData(const void* data, int32_t size, int32_t alignment, std::tuple<Effekseer::Backend::VertexBufferRef, int32_t>& result) override
	{
		if (size > size_)
		{
			return false;
		}

		int current_size = 16;
		for (int i = 0;;)
		{
			if (size <= current_size)
			{
				vertexBuffers_[i]->UpdateData(data, size, 0);
				result = std::tuple<Effekseer::Backend::VertexBufferRef, int32_t>(vertexBuffers_[i], 0);
				return true;
			}
			current_size *= 2;
		}

		return false;
	}
};

class VertexBufferRing : public VertexBuffer
{
	int currentIndex_ = 0;
	int offset_ = 0;
	int previous_offset_ = 0;
	Effekseer::CustomAlignedVector<uint8_t> buffer_;
	std::vector<Effekseer::Backend::VertexBufferRef> vertexBuffers_;

public:
	VertexBufferRing(Effekseer::Backend::GraphicsDeviceRef graphicsDevice, int32_t size, int32_t ringCount)
	{
		size_ = size;
		buffer_.resize(size_);

		for (int i = 0; i < ringCount; i++)
		{
			vertexBuffers_.emplace_back(graphicsDevice->CreateVertexBuffer(size_, nullptr, true));
		}
	}

	bool CanAllocate(int32_t size, int32_t alignment) override
	{
		return size + GetNextAliginedOffset(offset_, alignment) <= size_;
	}

	bool Allocate(int32_t size, int32_t alignment, std::tuple<void*, int32_t>& result) override
	{
		if (!CanAllocate(size, alignment))
		{
			return false;
		}

		auto next_offset = GetNextAliginedOffset(offset_, alignment);
		offset_ = next_offset + size;
		result = {buffer_.data() + next_offset, next_offset};
		return true;
	}

	Effekseer::Backend::VertexBufferRef Upload() override
	{
		vertexBuffers_[currentIndex_]->UpdateData(buffer_.data() + previous_offset_, offset_ - previous_offset_, previous_offset_);
		previous_offset_ = offset_;

		return vertexBuffers_[currentIndex_];
	}

	Effekseer::Backend::VertexBufferRef GetCurrentBuffer() override
	{
		return vertexBuffers_[currentIndex_];
	}

	void RenewBuffer() override
	{
		currentIndex_++;
		currentIndex_ %= vertexBuffers_.size();
		offset_ = 0;
		previous_offset_ = 0;
	}

	bool UpdateData(const void* data, int32_t size, int32_t alignment, std::tuple<Effekseer::Backend::VertexBufferRef, int32_t>& result) override
	{
		if (size > size_)
		{
			return false;
		}

		auto next_offset = GetNextAliginedOffset(offset_, alignment);

		if (size + next_offset > size_)
		{
			currentIndex_++;
			currentIndex_ %= vertexBuffers_.size();

			vertexBuffers_[currentIndex_]->UpdateData(data, size, 0);
			result = std::tuple<Effekseer::Backend::VertexBufferRef, int32_t>(vertexBuffers_[currentIndex_], 0);
			offset_ = size;
		}
		else
		{
			vertexBuffers_[currentIndex_]->UpdateData(data, size, next_offset);
			result = std::tuple<Effekseer::Backend::VertexBufferRef, int32_t>(vertexBuffers_[currentIndex_], next_offset);
			offset_ = next_offset + size;
		}

		return true;
	}
};

} // namespace EffekseerRenderer

#endif