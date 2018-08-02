
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
		glDeleteTextures(1, &texture);
	}

	bool RenderTextureGL::Initialize(int32_t width, int32_t height)
	{
		glGenTextures(1, &texture);

		if (glGetError() != GL_NO_ERROR) return false;

		glBindTexture(GL_TEXTURE_2D, texture);

		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGBA8,
			width,
			height,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			nullptr);
		
		this->width = width;
		this->height = height;

		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);

		return true;
	}

	DepthTextureGL::DepthTextureGL(Graphics* graphics)
	{

	}

	DepthTextureGL::~DepthTextureGL()
	{
		glDeleteTextures(1, &texture);
	}

	bool DepthTextureGL::Initialize(int32_t width, int32_t height)
	{
		glGenTextures(1, &texture);

		if (glGetError() != GL_NO_ERROR) return false;

		glBindTexture(GL_TEXTURE_2D, texture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);

		// Set filter
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_2D, 0);

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
		glDeleteTextures(1, &backTarget);

		recordingTarget.reset();
		recordingDepth.reset();

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

		glGenTextures(1, &backTarget);
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

		if (backTarget == 0 ||
			backTargetWidth != width ||
			backTargetHeight != height)
		{
			glBindTexture(GL_TEXTURE_2D, backTarget);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
			backTargetWidth = width;
			backTargetHeight = height;
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

		glBindTexture(GL_TEXTURE_2D, backTarget);
		//glCopyTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, 0, 0, width, height );
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, viewport[0], viewport[1], width, height);
		glBindTexture(GL_TEXTURE_2D, 0);

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

			GLuint cb[] = { 0 };
			GLuint db = 0;

			cb[0] = rt->GetBuffer();

			if (depthTexture != nullptr)
			{
				db = dt->GetBuffer();
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, cb[0], 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, db, 0);

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
		rt->Initialize(width, height);

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

		SaveTextureGL(pixels, recordingTarget->GetBuffer(), recordingWidth, recordingHeight);

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
		return (void*)backTarget;
	}

	EffekseerRenderer::Renderer* GraphicsGL::GetRenderer()
	{
		return renderer;
	}
}
