
#ifdef _WIN32
#define GLEW_STATIC 1
#include <GL/glew.h>
#endif

#include "efk.GraphicsGL.h"
#include <EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.GLExtension.h>
#include <EffekseerRendererGL/EffekseerRenderer/GraphicsDevice.h>

namespace efk
{
RenderTextureGL::RenderTextureGL(Graphics* graphics)
	: graphics(graphics)
{
}

RenderTextureGL::~RenderTextureGL()
{
	if (texture)
	{
		glDeleteTextures(1, &texture);
	}
	if (renderbuffer)
	{
		glDeleteRenderbuffers(1, &renderbuffer);
	}
}

bool RenderTextureGL::Initialize(Effekseer::Tool::Vector2DI size, TextureFormat format, uint32_t multisample)
{
	auto g = (GraphicsGL*)graphics;

	GLCheckError();

	GLint glInternalFormat;
	GLenum glFormat, glType;
	switch (format)
	{
	case TextureFormat::RGBA8U:
		glInternalFormat = GL_RGBA8;
		glFormat = GL_RGBA;
		glType = GL_UNSIGNED_BYTE;
		break;
	case TextureFormat::RGBA16F:
		glInternalFormat = GL_RGBA16F;
		glFormat = GL_RGBA;
		glType = GL_HALF_FLOAT;
		break;
	case TextureFormat::R16F:
		glInternalFormat = GL_R16F;
		glFormat = GL_RED;
		glType = GL_HALF_FLOAT;
		break;
	case TextureFormat::R32F:
		glInternalFormat = GL_R32F;
		glFormat = GL_RED;
		glType = GL_FLOAT;
		break;
	default:
		assert(0);
		return false;
	}

	if (multisample <= 1)
	{
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, glInternalFormat, size.X, size.Y, 0, glFormat, glType, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		// glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else
	{
		glGenRenderbuffers(1, &renderbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, multisample, glInternalFormat, size.X, size.Y);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}
	GLCheckError();

	this->size_ = size;
	this->samplingCount_ = multisample;
	this->format_ = format;
	this->texture_ = static_cast<EffekseerRendererGL::Backend::GraphicsDevice*>(g->GetGraphicsDevice().Get())->CreateTexture(texture, false, [] {});
	return true;
}

DepthTextureGL::DepthTextureGL(Graphics* graphics)
{
}

DepthTextureGL::~DepthTextureGL()
{
	if (texture)
	{
		glDeleteTextures(1, &texture);
	}
	if (renderbuffer)
	{
		glDeleteRenderbuffers(1, &renderbuffer);
	}
}

bool DepthTextureGL::Initialize(int32_t width, int32_t height, uint32_t multisample)
{
	if (glGetError() != GL_NO_ERROR)
		return false;

	if (multisample <= 1)
	{
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);
		// Set filter
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else
	{
		glGenRenderbuffers(1, &renderbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, multisample, GL_DEPTH_COMPONENT32, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}
	assert(glGetError() == GL_NO_ERROR);

	size_ = {width, height};

	return true;
}

bool SaveTextureGL(std::vector<Effekseer::Color>& dst, GLuint texture, int32_t width, int32_t height)
{
	GLCheckError();

	std::vector<Effekseer::Color> src;
	src.resize(width * height);
	dst.resize(width * height);

	glBindTexture(GL_TEXTURE_2D, texture);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, src.data());

	glBindTexture(GL_TEXTURE_2D, 0);

	auto size = width * height;
	dst.resize(size);

	for (auto y = 0; y < height; y++)
	{
		for (auto x = 0; x < width; x++)
		{
			dst[x + y * width] = src[x + (height - y - 1) * width];
		}
	}

	GLCheckError();

	return true;
}

GraphicsGL::GraphicsGL()
{
}

GraphicsGL::~GraphicsGL()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &frameBuffer);
	glDeleteFramebuffers(1, &frameBufferForCopySrc);
	glDeleteFramebuffers(1, &frameBufferForCopyDst);

	backTarget.reset();
}

bool GraphicsGL::Initialize(void* windowHandle, int32_t windowWidth, int32_t windowHeight, bool isSRGBMode)
{
	this->isSRGBMode = isSRGBMode;

	this->windowWidth = windowWidth;
	this->windowHeight = windowHeight;

	glDisable(GL_FRAMEBUFFER_SRGB);

	glGenFramebuffers(1, &frameBuffer);

	// for bug
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	GLCheckError();

	// TODO
	// create VAO

	// for glBlitFramebuffer
	glGenFramebuffers(1, &frameBufferForCopySrc);
	glGenFramebuffers(1, &frameBufferForCopyDst);

	graphicsDevice_ = Effekseer::MakeRefPtr<EffekseerRendererGL::Backend::GraphicsDevice>(EffekseerRendererGL::OpenGLDeviceType::OpenGL3);

	return true;
}

void GraphicsGL::CopyTo(RenderTexture* src, RenderTexture* dst)
{
	if (src->GetSize() != dst->GetSize())
		return;

	if (src->GetFormat() != dst->GetFormat())
		return;

	if (src->GetSamplingCount() != dst->GetSamplingCount())
	{
		ResolveRenderTarget(src, dst);
	}
	else
	{
		// Copy to background
		GLint backupFramebuffer;
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &backupFramebuffer);

		auto s = static_cast<RenderTextureGL*>(src);
		auto d = static_cast<RenderTextureGL*>(dst);

		if (s->GetSamplingCount() > 1)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, frameBufferForCopySrc);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, s->GetTexture());
		}
		else
		{
			glBindFramebuffer(GL_FRAMEBUFFER, frameBufferForCopySrc);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, s->GetTexture(), 0);
		}

		glBindTexture(GL_TEXTURE_2D, (GLuint)dst->GetViewID());
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, dst->GetSize().X, dst->GetSize().Y);
		glBindTexture(GL_TEXTURE_2D, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, backupFramebuffer);
	}
}

void GraphicsGL::Resize(int32_t width, int32_t height)
{
	this->windowWidth = width;
	this->windowHeight = height;
	ResetDevice();

	glViewport(0, 0, this->windowWidth, this->windowHeight);
}

bool GraphicsGL::Present()
{
	if (Presented != nullptr)
	{
		Presented();
	}

	return true;
}

void GraphicsGL::BeginScene()
{
}

void GraphicsGL::EndScene()
{
}

void GraphicsGL::SetRenderTarget(RenderTexture** renderTextures, int32_t renderTextureCount, DepthTexture* depthTexture)
{
	GLCheckError();

	// reset
	for (int32_t i = 0; i < 4; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glActiveTexture(GL_TEXTURE0);
	GLCheckError();

	if (renderTextures == nullptr || renderTextureCount == 0)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDrawBuffer(GL_BACK);
		glViewport(0, 0, windowWidth, windowHeight);
	}
	else
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

		auto rt = (RenderTextureGL*)renderTextures[0];
		auto dt = (DepthTextureGL*)depthTexture;

		for (size_t i = 0; i < renderTextureCount; i++)
		{
			auto rti = (RenderTextureGL*)renderTextures[i];

			if (rti == nullptr)
			{
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_RENDERBUFFER, 0);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, 0, 0);
			}
			else if (rti->GetSamplingCount() > 1)
			{
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_RENDERBUFFER, rti->GetBuffer());
			}
			else
			{
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, rti->GetTexture(), 0);
			}
		}

		for (size_t i = renderTextureCount; i < 4; i++)
		{
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_RENDERBUFFER, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, 0, 0);
		}

		if (rt->GetSamplingCount() > 1)
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, dt ? dt->GetBuffer() : 0);
		}
		else
		{
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, dt ? dt->GetTexture() : 0, 0);
		}

		static const GLenum bufs[] = {
			GL_COLOR_ATTACHMENT0,
			GL_COLOR_ATTACHMENT1,
			GL_COLOR_ATTACHMENT2,
			GL_COLOR_ATTACHMENT3,
		};
		glDrawBuffers(renderTextureCount, bufs);

		GLCheckError();

		glViewport(0, 0, renderTextures[0]->GetSize().X, renderTextures[0]->GetSize().Y);
	}

	currentRenderTargetCount_ = renderTextureCount;
	hasDepthBuffer_ = depthTexture != nullptr;
}

void GraphicsGL::SaveTexture(RenderTexture* texture, std::vector<Effekseer::Color>& pixels)
{
	auto t = static_cast<RenderTextureGL*>(texture);
	pixels.resize(t->GetSize().X * t->GetSize().Y);
	SaveTextureGL(pixels, t->GetTexture(), t->GetSize().X, t->GetSize().Y);
}

void GraphicsGL::Clear(Effekseer::Color color)
{
	GLCheckError();

	if (currentRenderTargetCount_ == 0)
	{
		GLbitfield bit = 0;
		{
			bit = bit | GL_COLOR_BUFFER_BIT;
			glClearColor(color.R / 255.0f, color.G / 255.0f, color.B / 255.0f, color.A / 255.0f);
		}

		{
			// Need that GL_DEPTH_TEST & WRITE are enabled
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);

			bit = bit | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
			glClearDepth(1.0f);
		}

		if (bit != 0)
		{
			glClear(bit);
		}
	}
	else
	{
		std::array<float, 4> colorf = {color.R / 255.0f, color.G / 255.0f, color.B / 255.0f, color.A / 255.0f};

		for (int32_t i = 0; i < currentRenderTargetCount_; i++)
		{
			glClearBufferfv(GL_COLOR, i, colorf.data());
			GLCheckError();
		}

		if (hasDepthBuffer_)
		{
			glDepthMask(GL_TRUE);
			float clearDepth[] = {1.0f};
			glClearBufferfv(GL_DEPTH, 0, clearDepth);
			GLCheckError();
		}
	}

	GLCheckError();
}

void GraphicsGL::ResolveRenderTarget(RenderTexture* src, RenderTexture* dest)
{
	auto rtSrc = (RenderTextureGL*)src;
	auto rtDest = (RenderTextureGL*)dest;

	GLint frameBufferBinding = 0;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &frameBufferBinding);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBufferForCopySrc);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferForCopyDst);
	GLCheckError();

	glFramebufferRenderbuffer(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rtSrc->GetBuffer());
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rtDest->GetTexture(), 0);
	GLCheckError();

	glBlitFramebuffer(0, 0, src->GetSize().X, src->GetSize().Y, 0, 0, dest->GetSize().X, dest->GetSize().Y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	GLCheckError();

	glFramebufferRenderbuffer(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, 0);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
	GLCheckError();

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferBinding);
}

void GraphicsGL::ResetDevice()
{
}

//void* GraphicsGL::GetBack()
//{
//	return (void*)(uintptr_t)backTarget->GetViewID();
//}

} // namespace efk
