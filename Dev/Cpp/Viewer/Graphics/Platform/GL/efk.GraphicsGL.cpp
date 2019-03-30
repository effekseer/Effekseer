
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

	bool RenderTextureGL::Initialize(int32_t width, int32_t height, TextureFormat format, uint32_t multisample)
	{
		if (glGetError() != GL_NO_ERROR) return false;

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
			glTexImage2D(GL_TEXTURE_2D, 0, glInternalFormat, width, height, 0, glFormat, glType, nullptr);
			glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		else
		{
			glGenRenderbuffers(1, &renderbuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, multisample, glInternalFormat, width, height);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
		}
		GLCheckError();

		this->width = width;
		this->height = height;
		this->multisample = multisample;

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
		if (glGetError() != GL_NO_ERROR) return false;

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

		glDeleteFramebuffers(1, &frameBufferForCopy);

		recordingTarget.reset();
		recordingDepth.reset();
		backTarget.reset();

		if (renderer != nullptr)
		{
			renderer->Destroy();
			renderer = nullptr;
		}
	}

	bool GraphicsGL::Initialize(void* windowHandle, int32_t windowWidth, int32_t windowHeight, bool isSRGBMode, int32_t spriteCount)
	{
		this->isSRGBMode = isSRGBMode;

		this->windowWidth = windowWidth;
		this->windowHeight = windowHeight;

		renderer = ::EffekseerRendererGL::Renderer::Create(spriteCount, EffekseerRendererGL::OpenGLDeviceType::OpenGL3);

		if (this->isSRGBMode)
		{
			glEnable(GL_FRAMEBUFFER_SRGB);
		}

		glGenFramebuffers(1, &frameBuffer);
		
		// for bug
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		GLCheckError();

		// TODO
		// create VAO

		glGenFramebuffers(1, &frameBufferForCopy);

		return true;
	}

	void GraphicsGL::CopyToBackground()
	{
#ifndef _WIN32
		GLint backupFramebuffer;
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &backupFramebuffer);
		if (backupFramebuffer <= 0) return;
#endif
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		uint32_t width = viewport[2];
		uint32_t height = viewport[3];

		if (!backTarget ||
			backTarget->GetWidth() != width ||
			backTarget->GetHeight() != height)
		{
			backTarget = std::make_shared<RenderTextureGL>(this);
			backTarget->Initialize(width, height, TextureFormat::RGBA8U);
		}

#ifndef _WIN32

		GLint rbtype;
		glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &rbtype);

		if (rbtype == GL_RENDERBUFFER) {
			GLint renderbuffer;
			glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &renderbuffer);

			glBindFramebuffer(GL_FRAMEBUFFER, frameBufferForCopy);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderbuffer);
		}
		else if (rbtype == GL_TEXTURE_2D) {
			GLint renderTexture;
			glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &renderTexture);

			glBindFramebuffer(GL_FRAMEBUFFER, frameBufferForCopy);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTexture, 0);
		}
#endif

		auto rt = (RenderTextureGL*)currentRenderTexture;
		if (rt->IsMultisample())
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

			if (rt->IsMultisample())
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

			glViewport(0, 0, renderTexture->GetWidth(), renderTexture->GetHeight());
		}
	}

	void GraphicsGL::BeginRecord(int32_t width, int32_t height)
	{
		auto rt = std::make_shared<RenderTextureGL>(this);
		rt->Initialize(width, height, TextureFormat::RGBA8U);

		auto dt = std::make_shared<DepthTextureGL>(this);
		dt->Initialize(width, height);

		recordingTarget = rt;
		recordingDepth = dt;

		recordingWidth = width;
		recordingHeight = height;

		SetRenderTarget(rt.get(), dt.get());
	}

	void GraphicsGL::EndRecord(std::vector<Effekseer::Color>& pixels)
	{
		pixels.resize(recordingWidth * recordingHeight);
		SetRenderTarget(nullptr, nullptr);

		SaveTextureGL(pixels, recordingTarget->GetTexture(), recordingWidth, recordingHeight);

		recordingTarget = nullptr;
		recordingDepth = nullptr;
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

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferForCopy);
		
		glFramebufferRenderbuffer(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rtSrc->GetBuffer());
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rtDest->GetTexture(), 0);

		glBlitFramebuffer(0, 0, src->GetWidth(), src->GetHeight(),
			0, 0, dest->GetWidth(), dest->GetHeight(), GL_COLOR_BUFFER_BIT, GL_NEAREST);

		glFramebufferRenderbuffer(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, 0);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
		
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		GLCheckError();
	}

	void GraphicsGL::ResetDevice()
	{
		renderer->OnLostDevice();

		if (LostedDevice != nullptr)
		{
			LostedDevice();
		}

		/*
		ES_SAFE_RELEASE(backTarget);
		ES_SAFE_RELEASE(backTargetTexture);

		

		HRESULT hr;

		D3DPRESENT_PARAMETERS d3dp;
		ZeroMemory(&d3dp, sizeof(d3dp));
		d3dp.BackBufferWidth = windowWidth;
		d3dp.BackBufferHeight = windowHeight;
		d3dp.BackBufferFormat = D3DFMT_X8R8G8B8;
		d3dp.BackBufferCount = 1;
		d3dp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dp.Windowed = TRUE;
		d3dp.hDeviceWindow = (HWND)windowHandle;
		d3dp.EnableAutoDepthStencil = TRUE;
		d3dp.AutoDepthStencilFormat = D3DFMT_D16;

		hr = d3d_device->Reset(&d3dp);

		if (FAILED(hr))
		{
			assert(0);
			return;
		}
		*/

		if (ResettedDevice != nullptr)
		{
			ResettedDevice();
		}

		renderer->OnResetDevice();
	}

	void* GraphicsGL::GetBack()
	{
		return (void*)(uintptr_t)backTarget->GetViewID();
	}

	EffekseerRenderer::Renderer* GraphicsGL::GetRenderer()
	{
		return renderer;
	}
}
