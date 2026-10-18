#include "VertexBuffer.h"

namespace EffekseerRenderer
{

VertexBufferMultiSize::VertexBufferMultiSize(Effekseer::Backend::GraphicsDeviceRef graphicsDevice, int32_t size)
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

bool VertexBufferMultiSize::GetIsValid() const
{
	for (auto v : vertexBuffers_)
	{
		if (v == nullptr)
		{
			return false;
		}
	}

	return true;
}

bool VertexBufferMultiSize::CanAllocate(int32_t size, int32_t alignment)
{
	return size + GetNextAliginedOffset(offset_, alignment) <= size_;
}

bool VertexBufferMultiSize::Allocate(int32_t size, int32_t alignment, std::tuple<void*, int32_t>& result)
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

Effekseer::Backend::VertexBufferRef VertexBufferMultiSize::Upload()
{
	int current_size = 16;
	for (int i = 0;;)
	{
		if (offset_ <= current_size)
		{
			vertexBuffers_[i]->UpdateData(buffer_.data(), offset_, 0);
			offset_ = 0;
			return vertexBuffers_[i];
		}
		i++;
		current_size *= 2;
	}

	return nullptr;
}

Effekseer::Backend::VertexBufferRef VertexBufferMultiSize::GetCurrentBuffer()
{
	return nullptr;
}

void VertexBufferMultiSize::RenewBuffer()
{
	offset_ = 0;
}

VertexBufferRing::VertexBufferRing(
	Effekseer::Backend::GraphicsDeviceRef graphicsDevice,
	int32_t size,
	int32_t ringCount,
	VertexBufferRingMode mode)
{
	graphicsDevice_ = graphicsDevice;
	size_ = size;
	mode_ = mode;
	buffer_.resize(size_);

	for (int i = 0; i < ringCount; i++)
	{
		vertexBuffers_.emplace_back(graphicsDevice->CreateVertexBuffer(size_, nullptr, true));
	}
}

bool VertexBufferRing::GetIsValid() const
{
	for (auto v : vertexBuffers_)
	{
		if (v == nullptr)
		{
			return false;
		}
	}

	return true;
}

bool VertexBufferRing::CanAllocate(int32_t size, int32_t alignment)
{
	return size + GetNextAliginedOffset(offset_, alignment) <= size_;
}

bool VertexBufferRing::Allocate(int32_t size, int32_t alignment, std::tuple<void*, int32_t>& result)
{
	if (!CanAllocate(size, alignment) || !EnsureCurrentBuffer())
	{
		return false;
	}

	auto next_offset = GetNextAliginedOffset(offset_, alignment);
	offset_ = next_offset + size;
	result = {buffer_.data() + next_offset, next_offset};
	return true;
}

Effekseer::Backend::VertexBufferRef VertexBufferRing::Upload()
{
	vertexBuffers_[currentIndex_]->UpdateData(buffer_.data() + previous_offset_, offset_ - previous_offset_, previous_offset_);
	previous_offset_ = offset_;

	return vertexBuffers_[currentIndex_];
}

Effekseer::Backend::VertexBufferRef VertexBufferRing::GetCurrentBuffer()
{
	return vertexBuffers_[currentIndex_];
}

bool VertexBufferRing::EnsureCurrentBuffer()
{
	while (currentIndex_ >= static_cast<int>(vertexBuffers_.size()))
	{
		auto buffer = graphicsDevice_->CreateVertexBuffer(size_, nullptr, true);
		if (!buffer) return false;
		vertexBuffers_.emplace_back(buffer);
	}
	return vertexBuffers_[currentIndex_] != nullptr;
}

void VertexBufferRing::BeginWriteForFrame(uint64_t frameNumber, int32_t frameCount)
{
	assert(frameNumber > 0 && frameCount > 0);
	assert(frameCount_ == 0 || frameCount_ == frameCount);
	if (frameNumber_ == frameNumber) return;
	frameNumber_ = frameNumber;
	frameCount_ = frameCount;
	currentIndex_ = static_cast<int>((frameNumber - 1) % frameCount);
	offset_ = 0;
	previous_offset_ = 0;
}

void VertexBufferRing::BeginWriteForCommandList()
{
	currentIndex_ = 0;
	offset_ = 0;
	previous_offset_ = 0;
}

void VertexBufferRing::RenewBuffer()
{
	// Each frame owns indices slot, slot + frameCount, ... including overflow.
	currentIndex_ += frameCount_ > 0 ? frameCount_ : 1;
	if (frameCount_ == 0 && mode_ == VertexBufferRingMode::Fixed)
	{
		currentIndex_ %= vertexBuffers_.size();
	}
	else
	{
		EnsureCurrentBuffer();
	}
	offset_ = 0;
	previous_offset_ = 0;
}

} // namespace EffekseerRenderer
