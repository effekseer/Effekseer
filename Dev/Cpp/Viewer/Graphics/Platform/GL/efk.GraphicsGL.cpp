
#ifdef _WIN32
#define GLEW_STATIC 1
#include <GL/glew.h>
#endif

#include "efk.GraphicsGL.h"

#include <EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.GLExtension.h>

namespace efk
{
RenderTextureGL::RenderTextureGL(Graphics* graphics)
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
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);
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
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, multisample, GL_DEPTH_COMPONENT24, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}
	assert(glGetError() == GL_NO_ERROR);

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
/*
void GraphicsGL::CopyToBackground()
{
#ifndef _WIN32
	GLint backupFramebuffer;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &backupFramebuffer);
	if (backupFramebuffer <= 0)
		return;
#endif
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	uint32_t width = viewport[2];
	uint32_t height = viewport[3];

	if (!backTarget || backTarget->GetSize().X != width || backTarget->GetSize().Y != height)
	{
		backTarget = std::make_shared<RenderTextureGL>(this);
		backTarget->Initialize(Effekseer::Tool::Vector2DI(width, height), TextureFormat::RGBA8U);
	}

#ifndef _WIN32

	GLint rbtype;
	glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &rbtype);

	if (rbtype == GL_RENDERBUFFER)
	{
		GLint renderbuffer;
		glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &renderbuffer);

		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferForCopySrc);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderbuffer);
	}
	else if (rbtype == GL_TEXTURE_2D)
	{
		GLint renderTexture;
		glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &renderTexture);

		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferForCopySrc);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTexture, 0);
	}
#endif

	auto rt = (RenderTextureGL*)currentRenderTexture;
	if (rt->GetSamplingCount() > 1)
	{
		ResolveRenderTarget(currentRenderTexture, backTarget.get());
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, (GLuint)backTarget->GetViewID());
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, viewport[0], viewport[1], width, height);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
#ifndef _WIN32
	glBindFramebuffer(GL_FRAMEBUFFER, backupFramebuffer);
#endif
}
*/

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

void GraphicsGL::SetRenderTarget(RenderTexture* renderTexture, DepthTexture* depthTexture)
{
	currentRenderTexture = renderTexture;
	currentDepthTexture = depthTexture;

	auto rt = (RenderTextureGL*)renderTexture;
	auto dt = (DepthTextureGL*)depthTexture;

	GLCheckError();

	// reset
	for (int32_t i = 0; i < 4; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glActiveTexture(GL_TEXTURE0);
	GLCheckError();

	if (renderTexture == nullptr)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDrawBuffer(GL_BACK);
		glViewport(0, 0, windowWidth, windowHeight);
	}
	else
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

		if (rt->GetSamplingCount() > 1)
		{
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rt->GetBuffer());
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, dt ? dt->GetBuffer() : 0);
		}
		else
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rt->GetTexture(), 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, dt ? dt->GetBuffer() : 0, 0);
		}

		static const GLenum bufs[] = {
			GL_COLOR_ATTACHMENT0,
		};
		glDrawBuffers(1, bufs);

		GLCheckError();

		glViewport(0, 0, renderTexture->GetSize().X, renderTexture->GetSize().Y);
	}
}

void GraphicsGL::SaveTexture(RenderTexture* texture, std::vector<Effekseer::Color>& pixels)
{
	auto t = static_cast<RenderTextureGL*>(texture);
	pixels.resize(t->GetSize().X * t->GetSize().Y);
	SaveTextureGL(pixels, t->GetTexture(), t->GetSize().X, t->GetSize().Y);
}

void GraphicsGL::Clear(Effekseer::Color color)
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
