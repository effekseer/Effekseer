#include "GraphicsDevice.h"

#include "EffekseerRendererGL.GLExtension.h"

namespace EffekseerRendererGL
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
	resources_.resize(static_cast<size_t>(size));
	GLExt::glGenBuffers(1, &buffer_);

	int arrayBufferBinding = 0;
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &arrayBufferBinding);

	GLExt::glBindBuffer(GL_ARRAY_BUFFER, buffer_);
	GLExt::glBufferData(GL_ARRAY_BUFFER, static_cast<uint32_t>(resources_.size()), nullptr, isDynamic_ ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	GLExt::glBindBuffer(GL_ARRAY_BUFFER, arrayBufferBinding);

	return true;
}

void VertexBuffer::Deallocate()
{
	if (buffer_ == 0)
	{
		GLExt::glDeleteBuffers(1, &buffer_);
		buffer_ = 0;
	}
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
	bool isSupportedBufferRange = GLExt::IsSupportedBufferRange();
#ifdef __ANDROID__
	isSupportedBufferRange = false;
#endif

	int arrayBufferBinding = 0;
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &arrayBufferBinding);

	GLExt::glBindBuffer(GL_ARRAY_BUFFER, buffer_);

	if (isSupportedBufferRange)
	{
		auto target = GLExt::glMapBufferRange(GL_ARRAY_BUFFER, offset, size, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
		memcpy(target, src, size);
		GLExt::glUnmapBuffer(GL_ARRAY_BUFFER);
	}
	else
	{
		memcpy(resources_.data() + offset, src, size);
		GLExt::glBufferData(GL_ARRAY_BUFFER, static_cast<uint32_t>(resources_.size()), resources_.data(), isDynamic_ ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	}

	GLExt::glBindBuffer(GL_ARRAY_BUFFER, arrayBufferBinding);
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
	resources_.resize(elementCount_ * stride_);
	GLExt::glGenBuffers(1, &buffer_);

	int elementArrayBufferBinding = 0;
	glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &elementArrayBufferBinding);

	GLExt::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_);
	GLExt::glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<uint32_t>(resources_.size()), nullptr, GL_STATIC_DRAW);
	GLExt::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementArrayBufferBinding);

	return true;
}

void IndexBuffer::Deallocate()
{
	if (buffer_ == 0)
	{
		GLExt::glDeleteBuffers(1, &buffer_);
		buffer_ = 0;
	}
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
	memcpy(resources_.data() + offset, src, size);

	int elementArrayBufferBinding = 0;
	glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &elementArrayBufferBinding);

	GLExt::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_);
	GLExt::glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<uint32_t>(resources_.size()), resources_.data(), GL_STATIC_DRAW);
	GLExt::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementArrayBufferBinding);
}

GraphicsDevice::GraphicsDevice(OpenGLDeviceType deviceType)
	: deviceType_(deviceType)
{
	GLExt::Initialize(deviceType);
}

GraphicsDevice::~GraphicsDevice()
{
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

OpenGLDeviceType GraphicsDevice::GetDeviceType() const
{
	return deviceType_;
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
} // namespace EffekseerRendererGL