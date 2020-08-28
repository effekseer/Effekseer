#include "GraphicsDevice.h"

namespace EffekseerRendererLLGI
{
namespace Backend
{

void DeviceObject::OnLostDevice()
{
}

void DeviceObject::OnResetDevice()
{
}

VertexBuffer::VertexBuffer(GraphicsDevice* graphicsDevice)
	: graphicsDevice_(graphicsDevice)
{
	ES_SAFE_ADDREF(graphicsDevice_);
	graphicsDevice_->Register(this);
}

VertexBuffer::~VertexBuffer()
{
	graphicsDevice_->Unregister(this);
	ES_SAFE_RELEASE(graphicsDevice_);
}

bool VertexBuffer::Allocate(int32_t size, bool isDynamic)
{
	buffer_ = LLGI::CreateSharedPtr(graphicsDevice_->GetGraphics()->CreateVertexBuffer(size));
	return true;
}

void VertexBuffer::Deallocate()
{
	buffer_.reset();
}

void VertexBuffer::OnLostDevice()
{
	Deallocate();
}

void VertexBuffer::OnResetDevice()
{
	Allocate(size_, isDynamic_);
}

bool VertexBuffer::Init(int32_t size, bool isDynamic)
{
	size_ = size;
	isDynamic_ = isDynamic;

	return Allocate(size_, isDynamic_);
}

void VertexBuffer::UpdateData(const void* src, int32_t size, int32_t offset)
{
	if (auto dst = static_cast<uint8_t*>(buffer_->Lock()))
	{
		memcpy(dst + offset, src, size);
		buffer_->Unlock();
	}
}

IndexBuffer::IndexBuffer(GraphicsDevice* graphicsDevice)
	: graphicsDevice_(graphicsDevice)
{
	ES_SAFE_ADDREF(graphicsDevice_);
	graphicsDevice_->Register(this);
}

IndexBuffer::~IndexBuffer()
{
	graphicsDevice_->Unregister(this);
	ES_SAFE_RELEASE(graphicsDevice_);
}

bool IndexBuffer::Allocate(int32_t elementCount, int32_t stride)
{
	buffer_ = LLGI::CreateSharedPtr(graphicsDevice_->GetGraphics()->CreateIndexBuffer(stride, elementCount));
	return buffer_ != nullptr;
}

void IndexBuffer::Deallocate()
{
	buffer_.reset();
}

void IndexBuffer::OnLostDevice()
{
	Deallocate();
}

void IndexBuffer::OnResetDevice()
{
	Allocate(elementCount_, stride_);
}

bool IndexBuffer::Init(int32_t elementCount, int32_t stride)
{
	elementCount_ = elementCount;
	stride_ = stride;

	return Allocate(elementCount_, stride_);
}

void IndexBuffer::UpdateData(const void* src, int32_t size, int32_t offset)
{
	if (auto dst = static_cast<uint8_t*>(buffer_->Lock()))
	{
		memcpy(dst + offset, src, size);
		buffer_->Unlock();
	}
}

GraphicsDevice::GraphicsDevice(LLGI::Graphics* graphics)
	: graphics_(graphics)
{
	ES_SAFE_ADDREF(graphics_);
}

GraphicsDevice::~GraphicsDevice()
{
	ES_SAFE_RELEASE(graphics_);
}

void GraphicsDevice::LostDevice()
{
	for (auto& o : objects_)
	{
		o->OnLostDevice();
	}
}

void GraphicsDevice::ResetDevice()
{
	for (auto& o : objects_)
	{
		o->OnResetDevice();
	}
}

LLGI::Graphics* GraphicsDevice::GetGraphics()
{
	return graphics_;
}

void GraphicsDevice::Register(DeviceObject* deviceObject)
{
	objects_.insert(deviceObject);
}

void GraphicsDevice::Unregister(DeviceObject* deviceObject)
{
	objects_.erase(deviceObject);
}

VertexBuffer* GraphicsDevice::CreateVertexBuffer(int32_t size, const void* initialData, bool isDynamic)
{
	auto ret = new VertexBuffer(this);

	if (!ret->Init(size, isDynamic))
	{
		ES_SAFE_RELEASE(ret);
		return nullptr;
	}

	ret->UpdateData(initialData, size, 0);

	return ret;
}

IndexBuffer* GraphicsDevice::CreateIndexBuffer(int32_t elementCount, const void* initialData, Effekseer::Backend::IndexBufferStrideType stride)
{
	auto ret = new IndexBuffer(this);

	if (!ret->Init(elementCount, stride == Effekseer::Backend::IndexBufferStrideType::Stride4 ? 4 : 2))
	{
		ES_SAFE_RELEASE(ret);
		return nullptr;
	}

	ret->UpdateData(initialData, elementCount * (stride == Effekseer::Backend::IndexBufferStrideType::Stride4 ? 4 : 2), 0);

	return ret;
}

} // namespace Backend
} // namespace EffekseerRendererLLGI