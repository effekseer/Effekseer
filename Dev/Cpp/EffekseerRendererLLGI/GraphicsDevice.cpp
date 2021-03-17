#include "GraphicsDevice.h"
#include <LLGI.Texture.h>

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

	elementCount_ = elementCount;
	strideType_ = stride == 4 ? Effekseer::Backend::IndexBufferStrideType::Stride4 : Effekseer::Backend::IndexBufferStrideType::Stride2;

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

Texture::Texture(GraphicsDevice* graphicsDevice)
	: graphicsDevice_(graphicsDevice)
{
	ES_SAFE_ADDREF(graphicsDevice_);
	graphicsDevice_->Register(this);
}

Texture::~Texture()
{
	if (onDisposed_)
	{
		onDisposed_();
	}
	graphicsDevice_->Unregister(this);
	ES_SAFE_RELEASE(graphicsDevice_);
}

bool Texture::Init(const Effekseer::Backend::TextureParameter& param)
{
	int mw = std::max(param.Size[0], param.Size[1]);
	int count = 1;

	while (mw > 1)
	{
		mw = mw << 1;
		count++;
	}

	LLGI::TextureInitializationParameter texParam;
	texParam.Size = LLGI::Vec2I(param.Size[0], param.Size[1]);
	texParam.MipMapCount = param.GenerateMipmap ? count : 1;

	// TODO : Fix it
	texParam.MipMapCount = 1;

	LLGI::TextureFormatType format = LLGI::TextureFormatType::R8G8B8A8_UNORM;

	if (param.Format == Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM)
	{
		texParam.Format = LLGI::TextureFormatType::R8G8B8A8_UNORM;
	}
	else if (param.Format == Effekseer::Backend::TextureFormatType::B8G8R8A8_UNORM)
	{
		texParam.Format = LLGI::TextureFormatType::B8G8R8A8_UNORM;
	}
	else if (param.Format == Effekseer::Backend::TextureFormatType::R8_UNORM)
	{
		texParam.Format = LLGI::TextureFormatType::R8_UNORM;
	}
	else if (param.Format == Effekseer::Backend::TextureFormatType::R16G16_FLOAT)
	{
		texParam.Format = LLGI::TextureFormatType::R16G16_FLOAT;
	}
	else if (param.Format == Effekseer::Backend::TextureFormatType::R16G16B16A16_FLOAT)
	{
		texParam.Format = LLGI::TextureFormatType::R16G16B16A16_FLOAT;
	}
	else if (param.Format == Effekseer::Backend::TextureFormatType::R32G32B32A32_FLOAT)
	{
		texParam.Format = LLGI::TextureFormatType::R32G32B32A32_FLOAT;
	}
	else if (param.Format == Effekseer::Backend::TextureFormatType::BC1)
	{
		texParam.Format = LLGI::TextureFormatType::BC1;
	}
	else if (param.Format == Effekseer::Backend::TextureFormatType::BC2)
	{
		texParam.Format = LLGI::TextureFormatType::BC2;
	}
	else if (param.Format == Effekseer::Backend::TextureFormatType::BC3)
	{
		texParam.Format = LLGI::TextureFormatType::BC3;
	}
	else if (param.Format == Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM_SRGB)
	{
		texParam.Format = LLGI::TextureFormatType::R8G8B8A8_UNORM_SRGB;
	}
	else if (param.Format == Effekseer::Backend::TextureFormatType::B8G8R8A8_UNORM_SRGB)
	{
		texParam.Format = LLGI::TextureFormatType::B8G8R8A8_UNORM_SRGB;
	}
	else if (param.Format == Effekseer::Backend::TextureFormatType::BC1_SRGB)
	{
		texParam.Format = LLGI::TextureFormatType::BC1_SRGB;
	}
	else if (param.Format == Effekseer::Backend::TextureFormatType::BC2_SRGB)
	{
		texParam.Format = LLGI::TextureFormatType::BC2_SRGB;
	}
	else if (param.Format == Effekseer::Backend::TextureFormatType::BC3_SRGB)
	{
		texParam.Format = LLGI::TextureFormatType::BC3_SRGB;
	}

	auto texture = graphicsDevice_->GetGraphics()->CreateTexture(texParam);
	auto buf = texture->Lock();

	if (param.InitialData.size() > 0)
	{
		memcpy(buf, param.InitialData.data(), param.InitialData.size());
	}

	texture->Unlock();

	texture_ = LLGI::CreateSharedPtr(texture);

	size_ = param.Size;
	type_ = Effekseer::Backend::TextureType::Color2D;

	return true;
}

bool Texture::Init(uint64_t id, std::function<void()> onDisposed)
{
	auto texture = graphicsDevice_->GetGraphics()->CreateTexture(id);
	if (texture == nullptr)
	{
		return false;
	}

	texture_ = LLGI::CreateSharedPtr(texture);
	onDisposed_ = onDisposed;

	type_ = Effekseer::Backend::TextureType::Color2D;

	return true;
}

bool Texture::Init(LLGI::Texture* texture)
{
	LLGI::SafeAddRef(texture);
	texture_ = LLGI::CreateSharedPtr(texture);
	type_ = Effekseer::Backend::TextureType::Color2D;
	auto size = texture_->GetSizeAs2D();
	size_ = {size.X, size.Y};
	return true;
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

Effekseer::Backend::VertexBufferRef GraphicsDevice::CreateVertexBuffer(int32_t size, const void* initialData, bool isDynamic)
{
	auto ret = Effekseer::MakeRefPtr<VertexBuffer>(this);

	if (!ret->Init(size, isDynamic))
	{
		return nullptr;
	}

	ret->UpdateData(initialData, size, 0);

	return ret;
}

Effekseer::Backend::IndexBufferRef GraphicsDevice::CreateIndexBuffer(int32_t elementCount, const void* initialData, Effekseer::Backend::IndexBufferStrideType stride)
{
	auto ret = Effekseer::MakeRefPtr<IndexBuffer>(this);

	if (!ret->Init(elementCount, stride == Effekseer::Backend::IndexBufferStrideType::Stride4 ? 4 : 2))
	{
		return nullptr;
	}

	ret->UpdateData(initialData, elementCount * (stride == Effekseer::Backend::IndexBufferStrideType::Stride4 ? 4 : 2), 0);

	return ret;
}

Effekseer::Backend::TextureRef GraphicsDevice::CreateTexture(const Effekseer::Backend::TextureParameter& param)
{
	auto ret = Effekseer::MakeRefPtr<Texture>(this);

	if (!ret->Init(param))
	{
		return nullptr;
	}

	return ret;
}

Effekseer::Backend::TextureRef GraphicsDevice::CreateTexture(uint64_t id, const std::function<void()>& onDisposed)
{
	auto ret = Effekseer::MakeRefPtr<Texture>(this);

	if (!ret->Init(id, onDisposed))
	{
		return nullptr;
	}

	return ret;
}

Effekseer::Backend::TextureRef GraphicsDevice::CreateTexture(LLGI::Texture* texture)
{
	auto ret = Effekseer::MakeRefPtr<Texture>(this);

	if (!ret->Init(texture))
	{
		return nullptr;
	}

	return ret;
}

} // namespace Backend
} // namespace EffekseerRendererLLGI