#include "GraphicsDeviceCPU.h"

namespace EffekseerRendererCPU
{
namespace Backend
{

VertexBuffer::VertexBuffer(int32_t size, const void* initialData, bool isDynamic)
{
	buffer_.resize(size);
	if (initialData != nullptr)
	{
		memcpy(buffer_.data(), initialData, buffer_.size());
	}
}

void VertexBuffer::UpdateData(const void* src, int32_t size, int32_t offset)
{
	memcpy(buffer_.data() + offset, src, size);
}

IndexBuffer::IndexBuffer(int32_t elementCount, const void* initialData, Effekseer::Backend::IndexBufferStrideType strideType)
{
	elementCount_ = elementCount;
	strideType_ = strideType;

	int32_t stride = strideType == Effekseer::Backend::IndexBufferStrideType::Stride4 ? 4 : 2;
	buffer_.resize(elementCount * stride);

	if (initialData != nullptr)
	{
		memcpy(buffer_.data(), initialData, buffer_.size());
	}
}

void IndexBuffer::UpdateData(const void* src, int32_t size, int32_t offset)
{
	memcpy(buffer_.data() + offset, src, size);
}

Effekseer::Backend::VertexBufferRef GraphicsDevice::CreateVertexBuffer(int32_t size, const void* initialData, bool isDynamic)
{
	return Effekseer::MakeRefPtr<VertexBuffer>(size, initialData, isDynamic);
}

Effekseer::Backend::IndexBufferRef GraphicsDevice::CreateIndexBuffer(int32_t elementCount, const void* initialData, Effekseer::Backend::IndexBufferStrideType stride)
{
	return Effekseer::MakeRefPtr<IndexBuffer>(elementCount, initialData, stride);
}

} // namespace Backend
} // namespace EffekseerRendererCPU