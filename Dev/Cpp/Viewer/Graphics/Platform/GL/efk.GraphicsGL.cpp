
#include "efk.GraphicsGL.h"
#include <EffekseerRendererGL/EffekseerRendererGL.GLExtension.h>
#include <EffekseerRendererGL/GraphicsDevice.h>
#include <OpenGLExtensions.h>

namespace GL = EffekseerRendererGL::GLExt;

namespace efk
{

bool SaveTextureGL(std::vector<Effekseer::Color>& dst, GLuint texture, int32_t width, int32_t height)
{
	GLCheckError();

	std::vector<Effekseer::Color> src;
	src.resize(width * height);
	dst.resize(width * height);

	glBindTexture(GL_TEXTURE_2D, texture);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	Effekseer::OpenGLHelper::glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, src.data());

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
	GL::glBindFramebuffer(GL_FRAMEBUFFER, 0);
	GL::glDeleteFramebuffers(1, &frameBuffer);
	GL::glDeleteFramebuffers(1, &frameBufferForCopySrc);
	GL::glDeleteFramebuffers(1, &frameBufferForCopyDst);
}

bool GraphicsGL::Initialize(void* windowHandle, int32_t windowWidth, int32_t windowHeight)
{
	this->windowWidth = windowWidth;
	this->windowHeight = windowHeight;

	glDisable(GL_FRAMEBUFFER_SRGB);

	graphicsDevice_ = Effekseer::MakeRefPtr<EffekseerRendererGL::Backend::GraphicsDevice>(EffekseerRendererGL::OpenGLDeviceType::OpenGL3);

	Effekseer::OpenGLHelper::Initialize();

	GL::glGenFramebuffers(1, &frameBuffer);

	// for bug
	GL::glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	GL::glBindFramebuffer(GL_FRAMEBUFFER, 0);
	GLCheckError();

	// TODO
	// create VAO

	// for glBlitFramebuffer
	GL::glGenFramebuffers(1, &frameBufferForCopySrc);
	GL::glGenFramebuffers(1, &frameBufferForCopyDst);

	return true;
}

void GraphicsGL::CopyTo(Effekseer::Backend::TextureRef src, Effekseer::Backend::TextureRef dst)
{
	if (src->GetParameter().Size != dst->GetParameter().Size)
		return;

	if (src->GetParameter().Format != dst->GetParameter().Format)
		return;

	if (src->GetParameter().SampleCount != dst->GetParameter().SampleCount)
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

		if (s->GetParameter().SampleCount > 1)
		{
			GL::glBindFramebuffer(GL_FRAMEBUFFER, frameBufferForCopySrc);
			Effekseer::OpenGLHelper::glFramebufferRenderbuffer(
				GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, s->GetRenderBuffer());
		}
		else
		{
			GL::glBindFramebuffer(GL_FRAMEBUFFER, frameBufferForCopySrc);
			GL::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, s->GetBuffer(), 0);
		}

		glBindTexture(GL_TEXTURE_2D, d->GetBuffer());
		Effekseer::OpenGLHelper::glCopyTexSubImage2D(
			GL_TEXTURE_2D, 0, 0, 0, 0, 0, dst->GetParameter().Size[0], dst->GetParameter().Size[1]);
		glBindTexture(GL_TEXTURE_2D, 0);

		GL::glBindFramebuffer(GL_FRAMEBUFFER, backupFramebuffer);
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

void GraphicsGL::SetRenderTarget(std::vector<Effekseer::Backend::TextureRef> renderTextures, Effekseer::Backend::TextureRef depthTexture)
{
	assert(renderTextures.size() > 0);
	GLCheckError();

	// reset
	for (int32_t i = 0; i < 4; i++)
	{
		GL::glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	GL::glActiveTexture(GL_TEXTURE0);
	GLCheckError();

	if (renderTextures[0] == nullptr)
	{
		GL::glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDrawBuffer(GL_BACK);
		glViewport(0, 0, windowWidth, windowHeight);
		currentRenderTargetCount_ = 0;
		hasDepthBuffer_ = true;
	}
	else
	{
		GL::glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

		auto rt = renderTextures[0].DownCast<EffekseerRendererGL::Backend::Texture>();
		auto dt = depthTexture.DownCast<EffekseerRendererGL::Backend::Texture>();

		for (size_t i = 0; i < renderTextures.size(); i++)
		{
			auto rti = renderTextures[i].DownCast<EffekseerRendererGL::Backend::Texture>();

			if (rti == nullptr)
			{
				Effekseer::OpenGLHelper::glFramebufferRenderbuffer(
					GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_RENDERBUFFER, 0);
				GL::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, 0, 0);
			}
			else if (rti->GetParameter().SampleCount > 1)
			{
				Effekseer::OpenGLHelper::glFramebufferRenderbuffer(
					GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_RENDERBUFFER, rti->GetRenderBuffer());
			}
			else
			{
				GL::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, rti->GetBuffer(), 0);
			}
		}

		for (size_t i = renderTextures.size(); i < 4; i++)
		{
			Effekseer::OpenGLHelper::glFramebufferRenderbuffer(
				GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_RENDERBUFFER, 0);
			GL::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, 0, 0);
		}

		if (dt != nullptr)
		{
			if (dt->GetParameter().SampleCount > 1)
			{
				Effekseer::OpenGLHelper::glFramebufferRenderbuffer(
					GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, dt->GetRenderBuffer());
			}
			else
			{
				GL::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, dt->GetBuffer(), 0);
			}
		}
		else
		{
			Effekseer::OpenGLHelper::glFramebufferRenderbuffer(
				GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
			GL::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
		}

		static const GLenum bufs[] = {
			GL_COLOR_ATTACHMENT0,
			GL_COLOR_ATTACHMENT1,
			GL_COLOR_ATTACHMENT2,
			GL_COLOR_ATTACHMENT3,
		};
		GL::glDrawBuffers(renderTextures.size(), bufs);

		GLCheckError();

		glViewport(0, 0, renderTextures[0]->GetParameter().Size[0], renderTextures[0]->GetParameter().Size[1]);
		currentRenderTargetCount_ = renderTextures.size();
		hasDepthBuffer_ = depthTexture != nullptr;
	}
}

void GraphicsGL::SaveTexture(Effekseer::Backend::TextureRef texture, std::vector<Effekseer::Color>& pixels)
{
	auto t = texture.DownCast<EffekseerRendererGL::Backend::Texture>();
	pixels.resize(t->GetParameter().Size[0] * t->GetParameter().Size[1]);
	SaveTextureGL(pixels, t->GetBuffer(), t->GetParameter().Size[0], t->GetParameter().Size[1]);
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
			Effekseer::OpenGLHelper::glClearBufferfv(GL_COLOR, i, colorf.data());
			GLCheckError();
		}

		if (hasDepthBuffer_)
		{
			glDepthMask(GL_TRUE);
			float clearDepth[] = {1.0f};
			Effekseer::OpenGLHelper::glClearBufferfv(GL_DEPTH, 0, clearDepth);
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
	GL::glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBufferForCopySrc);
	GL::glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferForCopyDst);
	GLCheckError();

	Effekseer::OpenGLHelper::glFramebufferRenderbuffer(
		GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rtSrc->GetRenderBuffer());
	GL::glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rtDest->GetBuffer(), 0);
	GLCheckError();

	Effekseer::OpenGLHelper::glBlitFramebuffer(
		0,
		0,
		src->GetParameter().Size[0],
		src->GetParameter().Size[1],
		0,
		0,
		dest->GetParameter().Size[0],
		dest->GetParameter().Size[1],
		GL_COLOR_BUFFER_BIT,
		GL_NEAREST);
	GLCheckError();

	Effekseer::OpenGLHelper::glFramebufferRenderbuffer(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, 0);
	GL::glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
	GLCheckError();

	GL::glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	GL::glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	GL::glBindFramebuffer(GL_FRAMEBUFFER, frameBufferBinding);
}

void GraphicsGL::ResetDevice()
{
}

} // namespace efk
