#include "GraphicsDevice.h"
#include "EffekseerRendererGL.Base.h"
#include "EffekseerRendererGL.GLExtension.h"

#ifdef __ANDROID__

#ifdef __ANDROID__DEBUG__
#include "android/log.h"
#define LOG(s) __android_log_print(ANDROID_LOG_DEBUG, "Tag", "%s", s)
#else
#define LOG(s) printf("%s", s)
#endif

#elif defined(_WIN32)
#include <windows.h>
#define LOG(s) OutputDebugStringA(s)
#else
#define LOG(s) printf("%s", s)
#endif

#undef min

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

	elementCount_ = elementCount;
	strideType_ = stride == 4 ? Effekseer::Backend::IndexBufferStrideType::Stride4 : Effekseer::Backend::IndexBufferStrideType::Stride2;
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

void UniformBuffer::UpdateData(const void* src, int32_t size, int32_t offset)
{
	assert(buffer_.size() >= size + offset && offset >= 0);

	buffer_.resize(size);

	if (auto data = static_cast<const uint8_t*>(src))
	{
		memcpy(buffer_.data() + offset, src, size);
	}
}

Texture::Texture(GraphicsDevice* graphicsDevice)
	: graphicsDevice_(graphicsDevice)
{
	ES_SAFE_ADDREF(graphicsDevice_);
}

Texture::~Texture()
{
	if (onDisposed_)
	{
		onDisposed_();
	}
	else
	{
		if (buffer_ > 0)
		{
			glDeleteTextures(1, &buffer_);
			buffer_ = 0;
		}
	}

	ES_SAFE_RELEASE(graphicsDevice_);
}

bool Texture::InitInternal(const Effekseer::Backend::TextureParameter& param)
{
	if (graphicsDevice_->GetDeviceType() == OpenGLDeviceType::OpenGL2 || graphicsDevice_->GetDeviceType() == OpenGLDeviceType::OpenGLES2)
	{
		if ((param.Format == Effekseer::Backend::TextureFormatType::B8G8R8A8_UNORM ||
			 param.Format == Effekseer::Backend::TextureFormatType::B8G8R8A8_UNORM_SRGB))
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
									  static_cast<GLsizei>(initialDataSize),
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

	size_ = param.Size;
	format_ = param.Format;
	hasMipmap_ = param.GenerateMipmap;

	return true;
}

bool Texture::Init(const Effekseer::Backend::TextureParameter& param)
{
	auto ret = InitInternal(param);

	type_ = Effekseer::Backend::TextureType::Color2D;

	return ret;
}

bool Texture::Init(const Effekseer::Backend::RenderTextureParameter& param)
{
	Effekseer::Backend::TextureParameter paramInternal;
	paramInternal.Size = param.Size;
	paramInternal.Format = param.Format;
	paramInternal.GenerateMipmap = false;
	auto ret = Init(paramInternal);

	type_ = Effekseer::Backend::TextureType::Render;

	return ret;
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

	size_ = param.Size;
	format_ = param.Format;
	hasMipmap_ = false;

	type_ = Effekseer::Backend::TextureType::Depth;

	return true;
}

bool Texture::Init(GLuint buffer, bool hasMipmap, const std::function<void()>& onDisposed)
{
	if (buffer == 0)
		return false;

	buffer_ = buffer;
	onDisposed_ = onDisposed;
	hasMipmap_ = hasMipmap;

	type_ = Effekseer::Backend::TextureType::Color2D;

	return true;
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

Shader::Shader(GraphicsDevice* graphicsDevice)
	: graphicsDevice_(graphicsDevice)
{
	ES_SAFE_ADDREF(graphicsDevice_);
}

Shader::~Shader()
{
	if (program_ > 0)
	{
		GLExt::glDeleteProgram(program_);
	}

	if (vao_ > 0)
	{
		GLExt::glDeleteVertexArrays(1, &vao_);
	}

	ES_SAFE_RELEASE(graphicsDevice_);
}

bool Shader::Init(const char* vsCode, const char* psCode, Effekseer::Backend::UniformLayoutRef& layout)
{
	GLint vsCodeLen = static_cast<GLint>(strlen(vsCode));
	GLint psCodeLen = static_cast<GLint>(strlen(psCode));

	GLint res_vs, res_fs, res_link = 0;
	auto vert_shader = GLExt::glCreateShader(GL_VERTEX_SHADER);

	GLExt::glShaderSource(vert_shader, 1, &vsCode, &vsCodeLen);
	GLExt::glCompileShader(vert_shader);
	GLExt::glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &res_vs);

	auto frag_shader = GLExt::glCreateShader(GL_FRAGMENT_SHADER);
	GLExt::glShaderSource(frag_shader, 1, &psCode, &psCodeLen);
	GLExt::glCompileShader(frag_shader);
	GLExt::glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &res_fs);

	// create shader program
	auto program = GLExt::glCreateProgram();
	GLExt::glAttachShader(program, vert_shader);
	GLExt::glAttachShader(program, frag_shader);

	// link shaders
	GLExt::glLinkProgram(program);
	GLExt::glGetProgramiv(program, GL_LINK_STATUS, &res_link);

#ifndef NDEBUG
	if (res_link == GL_FALSE)
	{
		// output errors
		char log[512];
		int32_t log_size;
		GLExt::glGetShaderInfoLog(vert_shader, sizeof(log), &log_size, log);
		if (log_size > 0)
		{
			LOG(": Vertex Shader error.\n");
			LOG(log);
		}
		GLExt::glGetShaderInfoLog(frag_shader, sizeof(log), &log_size, log);
		if (log_size > 0)
		{
			LOG(": Fragment Shader error.\n");
			LOG(log);
		}
		GLExt::glGetProgramInfoLog(program, sizeof(log), &log_size, log);
		if (log_size > 0)
		{
			LOG(": Shader Link error.\n");
			LOG(log);
		}
	}
#endif
	// dispose shader objects
	GLExt::glDeleteShader(frag_shader);
	GLExt::glDeleteShader(vert_shader);

	if (res_link == GL_FALSE)
	{
		GLExt::glDeleteProgram(program);
		return false;
	}

	program_ = program;

	if (GLExt::IsSupportedVertexArray())
	{
		GLExt::glGenVertexArrays(1, &vao_);
	}

	layout_ = layout;

	return true;
}

bool PipelineState::Init(const Effekseer::Backend::PipelineStateParameter& param)
{
	param_ = param;
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

bool RenderPass::Init(Effekseer::FixedSizeVector<Effekseer::Backend::TextureRef, Effekseer::Backend::RenderTargetMax>& textures, Effekseer::Backend::TextureRef depthTexture)
{
	for (int32_t i = 0; i < textures.size(); i++)
	{
		if (textures.at(i) == nullptr)
		{
			Effekseer::Log(Effekseer::LogType::Error, "RenderPass : textures " + std::to_string(i) + " must not be null.");
			return false;
		}

		if (textures.at(i)->GetTextureType() != Effekseer::Backend::TextureType::Render)
		{
			Effekseer::Log(Effekseer::LogType::Error, "RenderPass : textures " + std::to_string(i) + " must be Render.");
			return false;
		}
	}

	if (depthTexture != nullptr && depthTexture->GetTextureType() != Effekseer::Backend::TextureType::Depth)
	{
		Effekseer::Log(Effekseer::LogType::Error, "RenderPass : depthTexture must be Depth.");
		return false;
	}

	textures_ = textures;
	depthTexture_ = depthTexture;

	GLExt::glGenFramebuffers(1, &buffer_);
	if (buffer_ == 0)
	{
		return false;
	}

	GLint backupFramebuffer;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &backupFramebuffer);

	GLExt::glBindFramebuffer(GL_FRAMEBUFFER, buffer_);

	for (int32_t i = 0; i < textures.size(); i++)
	{
		GLExt::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, static_cast<Texture*>(textures.at(i).Get())->GetBuffer(), 0);
	}

	if (depthTexture != nullptr)
	{
		GLExt::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, static_cast<Texture*>(depthTexture.Get())->GetBuffer(), 0);
	}

	textures_ = textures;
	depthTexture_ = depthTexture;

	const GLenum bufs[] = {
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3,
	};
	GLExt::glDrawBuffers(static_cast<GLsizei>(textures.size()), bufs);

	GLExt::glBindFramebuffer(GL_FRAMEBUFFER, backupFramebuffer);
	return true;
}

GraphicsDevice::GraphicsDevice(OpenGLDeviceType deviceType, bool isExtensionsEnabled)
	: deviceType_(deviceType)
{
	GLExt::Initialize(deviceType, isExtensionsEnabled);

	if (deviceType == OpenGLDeviceType::OpenGL3 || deviceType == OpenGLDeviceType::OpenGLES3)
	{
		GLExt::glGenSamplers(Effekseer::TextureSlotMax, samplers_.data());
	}

	{
		GLint v;
		glGetIntegerv(GL_MAX_VARYING_VECTORS, &v);
		properties_[DevicePropertyType::MaxVaryingVectors] = v;
	}
}

GraphicsDevice::~GraphicsDevice()
{
	if (deviceType_ == OpenGLDeviceType::OpenGL3 || deviceType_ == OpenGLDeviceType::OpenGLES3)
	{
		GLExt::glDeleteSamplers(Effekseer::TextureSlotMax, samplers_.data());
	}
}

int GraphicsDevice::GetProperty(DevicePropertyType type) const
{
	return properties_.at(type);
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

Effekseer::Backend::TextureRef GraphicsDevice::CreateRenderTexture(const Effekseer::Backend::RenderTextureParameter& param)
{
	auto ret = Effekseer::MakeRefPtr<Texture>(this);

	if (!ret->Init(param))
	{
		return nullptr;
	}

	return ret;
}

Effekseer::Backend::TextureRef GraphicsDevice::CreateDepthTexture(const Effekseer::Backend::DepthTextureParameter& param)
{
	auto ret = Effekseer::MakeRefPtr<Texture>(this);

	if (!ret->Init(param))
	{
		return nullptr;
	}

	return ret;
}

Effekseer::Backend::UniformBufferRef GraphicsDevice::CreateUniformBuffer(int32_t size, const void* initialData)
{
	auto ret = Effekseer::MakeRefPtr<UniformBuffer>();

	if (!ret->Init(size, initialData))
	{
		return nullptr;
	}

	return ret;
}

Effekseer::Backend::VertexLayoutRef GraphicsDevice::CreateVertexLayout(const Effekseer::Backend::VertexLayoutElement* elements, int32_t elementCount)
{
	auto ret = Effekseer::MakeRefPtr<VertexLayout>();

	if (!ret->Init(elements, elementCount))
	{
		return nullptr;
	}

	return ret;
}

Effekseer::Backend::RenderPassRef GraphicsDevice::CreateRenderPass(Effekseer::FixedSizeVector<Effekseer::Backend::TextureRef, Effekseer::Backend::RenderTargetMax>& textures, Effekseer::Backend::TextureRef& depthTexture)
{
	auto ret = Effekseer::MakeRefPtr<RenderPass>(this);

	if (!ret->Init(textures, depthTexture))
	{
		return nullptr;
	}

	return ret;
}

Effekseer::Backend::PipelineStateRef GraphicsDevice::CreatePipelineState(const Effekseer::Backend::PipelineStateParameter& param)
{
	auto ret = Effekseer::MakeRefPtr<PipelineState>();

	if (!ret->Init(param))
	{
		return nullptr;
	}

	return ret;
}

Effekseer::Backend::ShaderRef GraphicsDevice::CreateShaderFromKey(const char* key)
{
	return nullptr;
}

Effekseer::Backend::ShaderRef GraphicsDevice::CreateShaderFromCodes(const char* vsCode, const char* psCode, Effekseer::Backend::UniformLayoutRef layout)
{
	auto ret = Effekseer::MakeRefPtr<Shader>(this);

	if (!ret->Init(vsCode, psCode, layout))
	{
		return nullptr;
	}

	return ret;
}

void GraphicsDevice::Draw(const Effekseer::Backend::DrawParameter& drawParam)
{
	if (drawParam.VertexBufferPtr == nullptr ||
		drawParam.IndexBufferPtr == nullptr ||
		drawParam.PipelineStatePtr == nullptr)
	{
		return;
	}

	auto pip = static_cast<PipelineState*>(drawParam.PipelineStatePtr.Get());
	auto shader = static_cast<Shader*>(pip->GetParam().ShaderPtr.Get());
	auto vertexLayout = static_cast<VertexLayout*>(pip->GetParam().VertexLayoutPtr.Get());

	if (shader->GetLayout() == nullptr)
	{
		return;
	}

	GLint currentVAO = 0;

	if (GLExt::IsSupportedVertexArray())
	{
		glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentVAO);
		GLExt::glBindVertexArray(shader->GetVAO());
	}

	GLExt::glBindBuffer(GL_ARRAY_BUFFER, static_cast<VertexBuffer*>(drawParam.VertexBufferPtr.Get())->GetBuffer());
	GLExt::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, static_cast<IndexBuffer*>(drawParam.IndexBufferPtr.Get())->GetBuffer());
	GLExt::glUseProgram(shader->GetProgram());

	// textures
	auto textureCount = std::min(static_cast<int32_t>(shader->GetLayout()->GetTextures().size()), drawParam.TextureCount);

	for (int32_t i = 0; i < textureCount; i++)
	{
		auto textureSlot = GLExt::glGetUniformLocation(shader->GetProgram(), shader->GetLayout()->GetTextures()[i].c_str());

		if (textureSlot < 0)
		{
			continue;
		}

		GLExt::glUniform1i(textureSlot, i);

		auto texture = static_cast<Texture*>(drawParam.TexturePtrs[i].Get());
		if (texture != nullptr)
		{
			GLExt::glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, texture->GetBuffer());

			static const GLint glfilterMin[] = {GL_NEAREST, GL_LINEAR_MIPMAP_LINEAR};
			static const GLint glfilterMin_NoneMipmap[] = {GL_NEAREST, GL_LINEAR};
			static const GLint glfilterMag[] = {GL_NEAREST, GL_LINEAR};
			static const GLint glwrap[] = {GL_REPEAT, GL_CLAMP_TO_EDGE};

			GLint filterMin = 0;
			GLint filterMag = 0;
			GLint wrap = 0;

			if (drawParam.TextureSamplingTypes[i] == Effekseer::Backend::TextureSamplingType::Linear)
			{
				if (texture->GetHasMipmap())
				{
					filterMin = glfilterMin[1];
				}
				else
				{
					filterMin = glfilterMin_NoneMipmap[1];
				}
				filterMag = glfilterMag[1];
			}
			else
			{
				if (texture->GetHasMipmap())
				{
					filterMin = glfilterMin[0];
				}
				else
				{
					filterMin = glfilterMin_NoneMipmap[0];
				}
				filterMag = glfilterMag[0];
			}

			if (drawParam.TextureWrapTypes[i] == Effekseer::Backend::TextureWrapType::Clamp)
			{
				wrap = glwrap[1];
			}
			else
			{
				wrap = glwrap[0];
			}

			if (deviceType_ == OpenGLDeviceType::OpenGL3 || deviceType_ == OpenGLDeviceType::OpenGLES3)
			{
				GLExt::glSamplerParameteri(samplers_[i], GL_TEXTURE_MAG_FILTER, filterMag);
				GLExt::glSamplerParameteri(samplers_[i], GL_TEXTURE_MIN_FILTER, filterMin);
				GLExt::glSamplerParameteri(samplers_[i], GL_TEXTURE_WRAP_S, wrap);
				GLExt::glSamplerParameteri(samplers_[i], GL_TEXTURE_WRAP_T, wrap);
				GLExt::glBindSampler(i, samplers_[i]);
			}
			else
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMag);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMin);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
			}
		}
	}

	// uniformss
	for (size_t i = 0; i < shader->GetLayout()->GetElements().size(); i++)
	{
		const auto& element = shader->GetLayout()->GetElements()[i];
		auto loc = GLExt::glGetUniformLocation(shader->GetProgram(), element.Name.c_str());

		if (loc < 0)
		{
			continue;
		}

		UniformBuffer* uniformBuffer = nullptr;

		if (element.Stage == Effekseer::Backend::ShaderStageType::Vertex)
		{
			uniformBuffer = static_cast<UniformBuffer*>(drawParam.VertexUniformBufferPtr.Get());
		}
		else if (element.Stage == Effekseer::Backend::ShaderStageType::Pixel)
		{
			uniformBuffer = static_cast<UniformBuffer*>(drawParam.PixelUniformBufferPtr.Get());
		}

		if (uniformBuffer != nullptr)
		{
			if (element.Type == Effekseer::Backend::UniformBufferLayoutElementType::Vector4)
			{
				const auto& buffer = uniformBuffer->GetBuffer();
				assert(buffer.size() >= element.Offset + sizeof(float) * 4);
				GLExt::glUniform4fv(loc, 1, reinterpret_cast<const GLfloat*>(buffer.data() + element.Offset));
			}
			else if (element.Type == Effekseer::Backend::UniformBufferLayoutElementType::Matrix44)
			{
				const auto& buffer = uniformBuffer->GetBuffer();
				assert(buffer.size() >= element.Offset + sizeof(float) * 4 * 4);
				GLExt::glUniformMatrix4fv(loc, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(buffer.data() + element.Offset));
			}
			else
			{
				// Unimplemented
				assert(0);
			}
		}
		else
		{
			// Invalid
			assert(0);
		}
	}

	GLCheckError();

	// layouts
	{
		int32_t vertexSize = 0;
		for (size_t i = 0; i < vertexLayout->GetElements().size(); i++)
		{
			const auto& element = vertexLayout->GetElements()[i];
			vertexSize += Effekseer::Backend::GetVertexLayoutFormatSize(element.Format);
		}

		uint32_t offset = 0;
		for (size_t i = 0; i < vertexLayout->GetElements().size(); i++)
		{
			const auto& element = vertexLayout->GetElements()[i];
			auto loc = GLExt::glGetAttribLocation(shader->GetProgram(), element.Name.c_str());

			GLenum type = {};
			int32_t count = {};
			GLboolean isNormalized = false;

			if (element.Format == Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UINT)
			{
				count = 4;
				type = GL_UNSIGNED_BYTE;
			}
			else if (element.Format == Effekseer::Backend::VertexLayoutFormat::R8G8B8A8_UNORM)
			{
				count = 4;
				type = GL_UNSIGNED_BYTE;
				isNormalized = GL_TRUE;
			}
			else if (element.Format == Effekseer::Backend::VertexLayoutFormat::R32_FLOAT)
			{
				count = 1;
				type = GL_FLOAT;
			}
			else if (element.Format == Effekseer::Backend::VertexLayoutFormat::R32G32_FLOAT)
			{
				count = 2;
				type = GL_FLOAT;
			}
			else if (element.Format == Effekseer::Backend::VertexLayoutFormat::R32G32B32_FLOAT)
			{
				count = 3;
				type = GL_FLOAT;
			}
			else if (element.Format == Effekseer::Backend::VertexLayoutFormat::R32G32B32A32_FLOAT)
			{
				count = 4;
				type = GL_FLOAT;
			}
			else
			{
				assert(0);
			}

			if (loc >= 0)
			{
				GLExt::glEnableVertexAttribArray(loc);
				GLExt::glVertexAttribPointer(loc,
											 count,
											 type,
											 isNormalized,
											 vertexSize,
											 reinterpret_cast<GLvoid*>(static_cast<size_t>(offset)));
			}

			offset += Effekseer::Backend::GetVertexLayoutFormatSize(element.Format);
		}
	}

	GLCheckError();

	// States
	GLint frontFace = 0;
	glGetIntegerv(GL_FRONT_FACE, &frontFace);

	if (GL_CW == frontFace)
	{
		if (pip->GetParam().Culling == Effekseer::Backend::CullingType::Clockwise)
		{
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
		}
		else if (pip->GetParam().Culling == Effekseer::Backend::CullingType::CounterClockwise)
		{
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
		}
		else if (pip->GetParam().Culling == Effekseer::Backend::CullingType::DoubleSide)
		{
			glDisable(GL_CULL_FACE);
			glCullFace(GL_FRONT_AND_BACK);
		}
	}
	else
	{
		if (pip->GetParam().Culling == Effekseer::Backend::CullingType::Clockwise)
		{
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
		}
		else if (pip->GetParam().Culling == Effekseer::Backend::CullingType::CounterClockwise)
		{
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
		}
		else if (pip->GetParam().Culling == Effekseer::Backend::CullingType::DoubleSide)
		{
			glDisable(GL_CULL_FACE);
			glCullFace(GL_FRONT_AND_BACK);
		}
	}

	GLCheckError();

	{
		if (pip->GetParam().IsDepthTestEnabled || pip->GetParam().IsDepthWriteEnabled)
		{
			glEnable(GL_DEPTH_TEST);
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
		}

		if (pip->GetParam().IsDepthTestEnabled)
		{
			std::array<GLenum, 8> depthFuncs;
			depthFuncs[static_cast<int>(::Effekseer::Backend::DepthFuncType::Less)] = GL_LESS;
			depthFuncs[static_cast<int>(::Effekseer::Backend::DepthFuncType::LessEqual)] = GL_LEQUAL;
			depthFuncs[static_cast<int>(::Effekseer::Backend::DepthFuncType::NotEqual)] = GL_NOTEQUAL;
			depthFuncs[static_cast<int>(::Effekseer::Backend::DepthFuncType::Equal)] = GL_EQUAL;
			depthFuncs[static_cast<int>(::Effekseer::Backend::DepthFuncType::Greater)] = GL_GREATER;
			depthFuncs[static_cast<int>(::Effekseer::Backend::DepthFuncType::GreaterEqual)] = GL_GEQUAL;
			depthFuncs[static_cast<int>(::Effekseer::Backend::DepthFuncType::Never)] = GL_NEVER;
			depthFuncs[static_cast<int>(::Effekseer::Backend::DepthFuncType::Always)] = GL_ALWAYS;

			glDepthFunc(depthFuncs[static_cast<int>(pip->GetParam().DepthFunc)]);
		}
		else
		{
			glDepthFunc(GL_ALWAYS);
		}

		glDepthMask(pip->GetParam().IsDepthWriteEnabled);
	}

	{
		if (pip->GetParam().IsBlendEnabled)
		{
			glEnable(GL_BLEND);

			std::array<GLenum, 5> blendEq;

			blendEq[static_cast<int>(::Effekseer::Backend::BlendEquationType::Add)] = GL_FUNC_ADD;
			blendEq[static_cast<int>(::Effekseer::Backend::BlendEquationType::Sub)] = GL_FUNC_SUBTRACT;
			blendEq[static_cast<int>(::Effekseer::Backend::BlendEquationType::ReverseSub)] = GL_FUNC_REVERSE_SUBTRACT;
			blendEq[static_cast<int>(::Effekseer::Backend::BlendEquationType::Min)] = GL_MIN;
			blendEq[static_cast<int>(::Effekseer::Backend::BlendEquationType::Max)] = GL_MAX;

			std::array<GLenum, 10> blendFunc;
			blendFunc[static_cast<int>(::Effekseer::Backend::BlendFuncType::Zero)] = GL_ZERO;
			blendFunc[static_cast<int>(::Effekseer::Backend::BlendFuncType::One)] = GL_ONE;
			blendFunc[static_cast<int>(::Effekseer::Backend::BlendFuncType::SrcColor)] = GL_SRC_COLOR;
			blendFunc[static_cast<int>(::Effekseer::Backend::BlendFuncType::OneMinusSrcColor)] = GL_ONE_MINUS_SRC_COLOR;
			blendFunc[static_cast<int>(::Effekseer::Backend::BlendFuncType::SrcAlpha)] = GL_SRC_ALPHA;
			blendFunc[static_cast<int>(::Effekseer::Backend::BlendFuncType::OneMinusSrcAlpha)] = GL_ONE_MINUS_SRC_ALPHA;
			blendFunc[static_cast<int>(::Effekseer::Backend::BlendFuncType::DstColor)] = GL_DST_COLOR;
			blendFunc[static_cast<int>(::Effekseer::Backend::BlendFuncType::OneMinusDstColor)] = GL_ONE_MINUS_DST_COLOR;
			blendFunc[static_cast<int>(::Effekseer::Backend::BlendFuncType::DstAlpha)] = GL_DST_ALPHA;
			blendFunc[static_cast<int>(::Effekseer::Backend::BlendFuncType::OneMinusDstAlpha)] = GL_ONE_MINUS_DST_ALPHA;

			GLExt::glBlendEquationSeparate(blendEq[static_cast<int>(pip->GetParam().BlendEquationRGB)], blendEq[static_cast<int>(pip->GetParam().BlendEquationAlpha)]);
			GLExt::glBlendFuncSeparate(blendFunc[static_cast<int>(pip->GetParam().BlendSrcFunc)], blendFunc[static_cast<int>(pip->GetParam().BlendDstFunc)], blendFunc[static_cast<int>(pip->GetParam().BlendSrcFuncAlpha)], blendFunc[static_cast<int>(pip->GetParam().BlendDstFuncAlpha)]);
		}
		else
		{
			glDisable(GL_BLEND);
		}
	}

	GLCheckError();

	GLenum primitiveMode = {};
	GLenum indexStrideType = {};
	int32_t indexPerPrimitive = {};

	if (pip->GetParam().Topology == Effekseer::Backend::TopologyType::Triangle)
	{
		primitiveMode = GL_TRIANGLES;
		indexPerPrimitive = 3;
	}
	else if (pip->GetParam().Topology == Effekseer::Backend::TopologyType::Line)
	{
		primitiveMode = GL_LINES;
		indexPerPrimitive = 2;
	}
	else if (pip->GetParam().Topology == Effekseer::Backend::TopologyType::Point)
	{
		primitiveMode = GL_POINTS;
		indexPerPrimitive = 1;
	}

	if (drawParam.IndexBufferPtr->GetStrideType() == Effekseer::Backend::IndexBufferStrideType::Stride4)
	{
		indexStrideType = GL_UNSIGNED_INT;
	}
	else if (drawParam.IndexBufferPtr->GetStrideType() == Effekseer::Backend::IndexBufferStrideType::Stride2)
	{
		indexStrideType = GL_UNSIGNED_SHORT;
	}

	if (drawParam.InstanceCount > 1)
	{
		GLExt::glDrawElementsInstanced(primitiveMode, indexPerPrimitive * drawParam.PrimitiveCount, indexStrideType, nullptr, drawParam.InstanceCount);
	}
	else
	{
		glDrawElements(primitiveMode, indexPerPrimitive * drawParam.PrimitiveCount, indexStrideType, nullptr);
	}

	for (size_t i = 0; i < vertexLayout->GetElements().size(); i++)
	{
		const auto& element = vertexLayout->GetElements()[i];
		auto loc = GLExt::glGetAttribLocation(shader->GetProgram(), element.Name.c_str());

		if (loc >= 0)
		{
			GLExt::glDisableVertexAttribArray(loc);
		}
	}

	if (GLExt::IsSupportedVertexArray())
	{
		GLExt::glBindVertexArray(currentVAO);
	}

	GLCheckError();
}

void GraphicsDevice::BeginRenderPass(Effekseer::Backend::RenderPassRef& renderPass, bool isColorCleared, bool isDepthCleared, Effekseer::Color clearColor)
{
	if (renderPass != nullptr)
	{
		GLExt::glBindFramebuffer(GL_FRAMEBUFFER, static_cast<RenderPass*>(renderPass.Get())->GetBuffer());
	}
	else
	{
		GLExt::glBindFramebuffer(GL_FRAMEBUFFER, 0);

#if !defined(EMSCRIPTEN) && !defined(__EFFEKSEER_RENDERER_GLES2__) && !defined(__EFFEKSEER_RENDERER_GL2__) && !defined(__EFFEKSEER_RENDERER_GLES3__)
		glDrawBuffer(GL_BACK);
#endif
	}

	GLbitfield flag = 0;

	if (isColorCleared)
	{
		flag |= GL_COLOR_BUFFER_BIT;
		glClearColor(clearColor.R / 255.0f, clearColor.G / 255.0f, clearColor.B / 255.0f, clearColor.A / 255.0f);
		GLCheckError();
	}

	if (isDepthCleared)
	{
		flag |= GL_DEPTH_BUFFER_BIT;
	}

	if (flag != 0)
	{
		glClear(flag);
	}

	GLCheckError();
}

void GraphicsDevice::EndRenderPass()
{
	GLExt::glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GLCheckError();
}

bool GraphicsDevice::UpdateUniformBuffer(Effekseer::Backend::UniformBufferRef& buffer, int32_t size, int32_t offset, const void* data)
{
	if (buffer == nullptr)
	{
		return false;
	}

	auto b = static_cast<UniformBuffer*>(buffer.Get());

	b->UpdateData(data, size, offset);

	return true;
}

Effekseer::Backend::TextureRef GraphicsDevice::CreateTexture(GLuint buffer, bool hasMipmap, const std::function<void()>& onDisposed)
{
	auto ret = Effekseer::MakeRefPtr<Texture>(this);

	if (!ret->Init(buffer, hasMipmap, onDisposed))
	{
		return nullptr;
	}

	return ret;
}

} // namespace Backend
} // namespace EffekseerRendererGL