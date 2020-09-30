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

bool UniformBuffer::Init(int32_t size, const void* initialData)
{
	buffer_.resize(size);

	if (auto data = static_cast<const uint8_t*>(initialData))
	{
		buffer_.assign(data, data + size);
	}

	return true;
}

Texture::Texture(GraphicsDevice* graphicsDevice)
	: graphicsDevice_(graphicsDevice)
{
	ES_SAFE_ADDREF(graphicsDevice_);
}

Texture::~Texture()
{
	if (buffer_ > 0)
	{
		glDeleteTextures(1, &buffer_);
		buffer_ = 0;
	}

	ES_SAFE_RELEASE(graphicsDevice_);
}

bool Texture::InitInternal(const Effekseer::Backend::TextureParameter& param)
{
	if (graphicsDevice_->GetDeviceType() == OpenGLDeviceType::OpenGL2 || graphicsDevice_->GetDeviceType() == OpenGLDeviceType::OpenGLES2)
	{
		if (param.Format == Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM ||
			param.Format == Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM_SRGB)
		{
			// not supported
			return false;
		}
	}

	GLint bound = 0;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound);

	glGenTextures(1, &buffer_);
	glBindTexture(GL_TEXTURE_2D, buffer_);

	// Compressed texture
	auto isCompressed = param.Format == Effekseer::Backend::TextureFormatType::BC1 ||
						param.Format == Effekseer::Backend::TextureFormatType::BC2 ||
						param.Format == Effekseer::Backend::TextureFormatType::BC3 ||
						param.Format == Effekseer::Backend::TextureFormatType::BC1_SRGB ||
						param.Format == Effekseer::Backend::TextureFormatType::BC2_SRGB ||
						param.Format == Effekseer::Backend::TextureFormatType::BC3_SRGB;

	const size_t initialDataSize = param.InitialData.size();
	const void* initialDataPtr = param.InitialData.size() > 0 ? param.InitialData.data() : nullptr;

	if (isCompressed)
	{
		GLenum format = GL_RGBA;

		if (param.Format == Effekseer::Backend::TextureFormatType::BC1)
		{
			format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		}
		else if (param.Format == Effekseer::Backend::TextureFormatType::BC2)
		{
			format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		}
		else if (param.Format == Effekseer::Backend::TextureFormatType::BC3)
		{
			format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		}
		else if (param.Format == Effekseer::Backend::TextureFormatType::BC1_SRGB)
		{
			format = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT;
		}
		else if (param.Format == Effekseer::Backend::TextureFormatType::BC2_SRGB)
		{
			format = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT;
		}
		else if (param.Format == Effekseer::Backend::TextureFormatType::BC3_SRGB)
		{
			format = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
		}

		GLExt::glCompressedTexImage2D(GL_TEXTURE_2D,
									  0,
									  format,
									  param.Size[0],
									  param.Size[1],
									  0,
									  initialDataSize,
									  initialDataPtr);
	}
	else
	{
		GLuint internalFormat = GL_RGBA;
		GLenum format = GL_RGBA;
		GLenum type = GL_UNSIGNED_BYTE;

		if (param.Format == Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM)
		{
			internalFormat = GL_RGBA;
			format = GL_RGBA;
			type = GL_UNSIGNED_BYTE;
		}
		else if (param.Format == Effekseer::Backend::TextureFormatType::B8G8R8A8_UNORM)
		{
			internalFormat = GL_RGBA;
			format = GL_BGRA;
			type = GL_UNSIGNED_BYTE;
		}
		else if (param.Format == Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM_SRGB)
		{
			internalFormat = GL_SRGB8_ALPHA8;
			format = GL_RGBA;
			type = GL_UNSIGNED_BYTE;
		}
		else if (param.Format == Effekseer::Backend::TextureFormatType::B8G8R8A8_UNORM_SRGB)
		{
			internalFormat = GL_SRGB8_ALPHA8;
			format = GL_BGRA;
			type = GL_UNSIGNED_BYTE;
		}
		else if (param.Format == Effekseer::Backend::TextureFormatType::R8_UNORM)
		{
			internalFormat = GL_R8;
			format = GL_RED;
			type = GL_UNSIGNED_BYTE;
		}
		else if (param.Format == Effekseer::Backend::TextureFormatType::R16G16_FLOAT)
		{
			internalFormat = GL_RG16F;
			format = GL_RG;
			type = GL_HALF_FLOAT;
		}
		else if (param.Format == Effekseer::Backend::TextureFormatType::R16G16B16A16_FLOAT)
		{
			internalFormat = GL_RGBA16F;
			format = GL_RGBA;
			type = GL_HALF_FLOAT;
		}
		else if (param.Format == Effekseer::Backend::TextureFormatType::R32G32B32A32_FLOAT)
		{
			internalFormat = GL_RGBA32F;
			format = GL_RGBA;
			type = GL_FLOAT;
		}

		glTexImage2D(GL_TEXTURE_2D,
					 0,
					 internalFormat,
					 param.Size[0],
					 param.Size[1],
					 0,
					 format,
					 type,
					 initialDataPtr);
	}

	if (param.GenerateMipmap)
	{
		GLExt::glGenerateMipmap(GL_TEXTURE_2D);
	}

	glBindTexture(GL_TEXTURE_2D, bound);

	return true;
}

bool Texture::Init(const Effekseer::Backend::TextureParameter& param)
{
	return InitInternal(param);
}

bool Texture::Init(const Effekseer::Backend::RenderTextureParameter& param)
{
	Effekseer::Backend::TextureParameter paramInternal;
	paramInternal.Size = param.Size;
	paramInternal.Format = param.Format;
	paramInternal.GenerateMipmap = false;
	return Init(paramInternal);
}

bool Texture::Init(const Effekseer::Backend::DepthTextureParameter& param)
{
	GLint bound = 0;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound);

	glGenTextures(1, &buffer_);
	glBindTexture(GL_TEXTURE_2D, buffer_);

	if (param.Format == Effekseer::Backend::TextureFormatType::D24S8)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, param.Size[0], param.Size[1], 0, GL_DEPTH_STENCIL, GL_FLOAT, nullptr);
	}
	else if (param.Format == Effekseer::Backend::TextureFormatType::D32S8)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH32F_STENCIL8, param.Size[0], param.Size[1], 0, GL_DEPTH_STENCIL, GL_FLOAT, nullptr);
	}
	else if (param.Format == Effekseer::Backend::TextureFormatType::D32S8)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, param.Size[0], param.Size[1], 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	}
	else
	{
		glDeleteTextures(1, &buffer_);
		buffer_ = 0;
		glBindTexture(GL_TEXTURE_2D, bound);
		return false;
	}

	glBindTexture(GL_TEXTURE_2D, bound);

	return false;
}

bool VertexLayout::Init(const Effekseer::Backend::VertexLayoutElement* elements, int32_t elementCount)
{
	elements_.resize(elementCount);

	for (int32_t i = 0; i < elementCount; i++)
	{
		elements_[i] = elements[i];
	}

	return true;
}

RenderPass::RenderPass(GraphicsDevice* graphicsDevice)
	: graphicsDevice_(graphicsDevice)
{
	ES_SAFE_ADDREF(graphicsDevice_);
}

RenderPass::~RenderPass()
{
	if (buffer_ > 0)
	{
		GLExt::glDeleteFramebuffers(1, &buffer_);
		buffer_ = 0;
	}

	ES_SAFE_RELEASE(graphicsDevice_);
}

bool RenderPass::Init(Texture** textures, int32_t textureCount, Texture* depthTexture)
{
	GLExt::glGenFramebuffers(1, &buffer_);
	if (buffer_ == 0)
	{
		return false;
	}

	GLint backupFramebuffer;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &backupFramebuffer);

	GLExt::glBindFramebuffer(GL_FRAMEBUFFER, buffer_);

	for (int32_t i = 0; i < textureCount; i++)
	{
		GLExt::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textures[i]->GetBuffer(), 0);
	}

	if (depthTexture != nullptr)
	{
		GLExt::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture->GetBuffer(), 0);
	}

	textures_.resize(textureCount);
	for (int32_t i = 0; i < textureCount; i++)
	{
		textures_[i] = Effekseer::CreateReference(textures[i], true);
	}

	depthTexture_ = Effekseer::CreateReference(depthTexture, true);

	GLExt::glBindFramebuffer(GL_FRAMEBUFFER, backupFramebuffer);
	return true;
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

Texture* GraphicsDevice::CreateTexture(const Effekseer::Backend::TextureParameter& param)
{
	auto ret = new Texture(this);

	if (!ret->Init(param))
	{
		ES_SAFE_RELEASE(ret);
		return nullptr;
	}

	return ret;
}

Texture* GraphicsDevice::CreateRenderTexture(const Effekseer::Backend::RenderTextureParameter& param)
{
	auto ret = new Texture(this);

	if (!ret->Init(param))
	{
		ES_SAFE_RELEASE(ret);
		return nullptr;
	}

	return ret;
}

Texture* GraphicsDevice::CreateDepthTexture(const Effekseer::Backend::DepthTextureParameter& param)
{
	auto ret = new Texture(this);

	if (!ret->Init(param))
	{
		ES_SAFE_RELEASE(ret);
		return nullptr;
	}

	return ret;
}

UniformBuffer* GraphicsDevice::CreateUniformBuffer(int32_t size, const void* initialData)
{
	auto ret = new UniformBuffer();

	if (!ret->Init(size, initialData))
	{
		ES_SAFE_RELEASE(ret);
		return nullptr;
	}

	return ret;
}

VertexLayout* GraphicsDevice::CreateVertexLayout(const Effekseer::Backend::VertexLayoutElement* elements, int32_t elementCount)
{
	auto ret = new VertexLayout();

	if (!ret->Init(elements, elementCount))
	{
		ES_SAFE_RELEASE(ret);
		return nullptr;
	}

	return ret;
}

RenderPass* GraphicsDevice::CreateRenderPass(Effekseer::Backend::Texture** textures, int32_t textureCount, Effekseer::Backend::Texture* depthTexture)
{
	auto ret = new RenderPass(this);

	if (!ret->Init(reinterpret_cast<Texture**>(textures), textureCount, reinterpret_cast<Texture*>(depthTexture)))
	{
		ES_SAFE_RELEASE(ret);
		return nullptr;
	}

	return ret;
}

} // namespace Backend
} // namespace EffekseerRendererGL