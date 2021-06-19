
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
}

bool RenderTextureGL::Initialize(Effekseer::Tool::Vector2DI size, Effekseer::Backend::TextureFormatType format, uint32_t multisample)
{
	auto g = (GraphicsGL*)graphics;
	auto gd = g->GetGraphicsDevice().DownCast<EffekseerRendererGL::Backend::GraphicsDevice>();

	Effekseer::Backend::RenderTextureParameter param;
	param.Format = format;
	param.SamplingCount = multisample;
	param.Size = {size.X, size.Y};
	texture_ = gd->CreateRenderTexture(param).DownCast<EffekseerRendererGL::Backend::Texture>();

	this->size_ = size;
	this->samplingCount_ = multisample;
	this->format_ = format;

	if (multisample <= 1 && texture_ != nullptr)
	{
		GLint bound;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound);
		glBindTexture(GL_TEXTURE_2D, texture_->GetBuffer());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, bound);
	}

	return texture_ != nullptr;
}

DepthTextureGL::DepthTextureGL(Graphics* graphics)
	: graphics_(graphics)
{
}

DepthTextureGL::~DepthTextureGL()
{
}

bool DepthTextureGL::Initialize(int32_t width, int32_t height, uint32_t multisample)
{
	if (glGetError() != GL_NO_ERROR)
		return false;
	auto g = (GraphicsGL*)graphics_;
	auto gd = g->GetGraphicsDevice().DownCast<EffekseerRendererGL::Backend::GraphicsDevice>();

	Effekseer::Backend::DepthTextureParameter param;
	param.Format = Effekseer::Backend::TextureFormatType::D32;
	param.SamplingCount = multisample;
	param.Size = {width, height};
	texture_ = gd->CreateDepthTexture(param).DownCast<EffekseerRendererGL::Backend::Texture>();

	if (multisample <= 1 && texture_ != nullptr)
	{
		glBindTexture(GL_TEXTURE_2D, texture_->GetBuffer());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	return texture_ != nullptr;
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

bool GraphicsGL::Initialize(void* windowHandle, int32_t windowWidth, int32_t windowHeight)
{
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

void GraphicsGL::CopyTo(Effekseer::Backend::TextureRef src, Effekseer::Backend::TextureRef dst)
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

		auto s = src.DownCast<EffekseerRendererGL::Backend::Texture>();
		auto d = dst.DownCast<EffekseerRendererGL::Backend::Texture>();

		if (s->GetSamplingCount() > 1)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, frameBufferForCopySrc);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, s->GetRenderBuffer());
		}
		else
		{
			glBindFramebuffer(GL_FRAMEBUFFER, frameBufferForCopySrc);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, s->GetBuffer(), 0);
		}

		glBindTexture(GL_TEXTURE_2D, d->GetBuffer());
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, dst->GetSize()[0], dst->GetSize()[1]);
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

void GraphicsGL::SetRenderTarget(std::vector<Effekseer::Backend::TextureRef> renderTextures, Effekseer::Backend::TextureRef depthTexture)
{
	assert(renderTextures.size() > 0);
	GLCheckError();
	
	// reset
	for (int32_t i = 0; i < 4; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glActiveTexture(GL_TEXTURE0);
	GLCheckError();

	if (renderTextures[0] == nullptr)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDrawBuffer(GL_BACK);
		glViewport(0, 0, windowWidth, windowHeight);
		currentRenderTargetCount_ = 0;
		hasDepthBuffer_ = true;
	}
	else
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

		auto rt = renderTextures[0].DownCast<EffekseerRendererGL::Backend::Texture>();
		auto dt = depthTexture.DownCast<EffekseerRendererGL::Backend::Texture>();

		for (size_t i = 0; i < renderTextures.size(); i++)
		{
			auto rti = renderTextures[i].DownCast<EffekseerRendererGL::Backend::Texture>();

			if (rti == nullptr)
			{
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_RENDERBUFFER, 0);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, 0, 0);
			}
			else if (rti->GetSamplingCount() > 1)
			{
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_RENDERBUFFER, rti->GetRenderBuffer());
			}
			else
			{
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, rti->GetBuffer(), 0);
			}
		}

		for (size_t i = renderTextures.size(); i < 4; i++)
		{
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_RENDERBUFFER, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, 0, 0);
		}

		if (dt != nullptr)
		{
			if (dt->GetSamplingCount() > 1)
			{
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, dt->GetRenderBuffer());
			}
			else
			{
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, dt->GetBuffer(), 0);
			}
		}
		else
		{
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
		}

		static const GLenum bufs[] = {
			GL_COLOR_ATTACHMENT0,
			GL_COLOR_ATTACHMENT1,
			GL_COLOR_ATTACHMENT2,
			GL_COLOR_ATTACHMENT3,
		};
		glDrawBuffers(renderTextures.size(), bufs);

		GLCheckError();

		glViewport(0, 0, renderTextures[0]->GetSize()[0], renderTextures[0]->GetSize()[1]);
		currentRenderTargetCount_ = renderTextures.size();
		hasDepthBuffer_ = depthTexture != nullptr;
	}
}

void GraphicsGL::SaveTexture(Effekseer::Backend::TextureRef texture, std::vector<Effekseer::Color>& pixels)
{
	auto t = texture.DownCast<EffekseerRendererGL::Backend::Texture>();
	pixels.resize(t->GetSize()[0] * t->GetSize()[1]);
	SaveTextureGL(pixels, t->GetBuffer(), t->GetSize()[0], t->GetSize()[1]);
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

void GraphicsGL::ResolveRenderTarget(Effekseer::Backend::TextureRef src, Effekseer::Backend::TextureRef dest)
{
	auto rtSrc = src.DownCast<EffekseerRendererGL::Backend::Texture>();
	auto rtDest = dest.DownCast<EffekseerRendererGL::Backend::Texture>();

	GLint frameBufferBinding = 0;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &frameBufferBinding);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBufferForCopySrc);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferForCopyDst);
	GLCheckError();

	glFramebufferRenderbuffer(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rtSrc->GetRenderBuffer());
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rtDest->GetBuffer(), 0);
	GLCheckError();

	glBlitFramebuffer(0, 0, src->GetSize()[0], src->GetSize()[1], 0, 0, dest->GetSize()[0], dest->GetSize()[1], GL_COLOR_BUFFER_BIT, GL_NEAREST);
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

} // namespace efk
