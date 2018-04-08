
#define GLEW_STATIC 1
#include <GL/glew.h>

#include "efk.GraphicsGL.h"

#include <EffekseerRenderer/EffekseerRendererGL.GLExtension.h>

namespace efk
{
	RenderTextureGL::RenderTextureGL()
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

	DepthTextureGL::DepthTextureGL()
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

	GraphicsGL::GraphicsGL()
	{

	}

	GraphicsGL::~GraphicsGL()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteFramebuffers(1, &frameBuffer);
		/*
		if (renderer != nullptr)
		{
			renderer->Destroy();
			renderer = nullptr;
		}

		assert(renderDefaultTarget == nullptr);
		assert(recordingTarget == nullptr);
		
		ES_SAFE_RELEASE(backTarget);
		ES_SAFE_RELEASE(backTargetTexture);

		ES_SAFE_RELEASE(d3d_device);
		ES_SAFE_RELEASE(d3d);
		*/
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
		return true;
	}

	void GraphicsGL::CopyToBackground()
	{
		return;
		/*
		bool ret = false;

		HRESULT hr;

		IDirect3DSurface9* tempRender = nullptr;
		IDirect3DSurface9* tempDepth = nullptr;

		hr = d3d_device->GetRenderTarget(0, &tempRender);
		if (FAILED(hr))
		{
			goto Exit;
		}

		hr = d3d_device->GetDepthStencilSurface(&tempDepth);
		if (FAILED(hr))
		{
			goto Exit;
		}

		D3DSURFACE_DESC temp_desc;
		D3DSURFACE_DESC background_desc;

		tempRender->GetDesc(&temp_desc);

		if (backTarget != nullptr)
		{
			backTarget->GetDesc(&background_desc);
		}

		if (backTarget == nullptr || temp_desc.Width != background_desc.Width || temp_desc.Height != background_desc.Height)
		{
			ES_SAFE_RELEASE(backTarget);
			ES_SAFE_RELEASE(backTargetTexture);

			hr = d3d_device->CreateTexture(
				temp_desc.Width,
				temp_desc.Height,
				1,
				D3DUSAGE_RENDERTARGET,
				D3DFMT_A8R8G8B8,
				D3DPOOL_DEFAULT,
				&backTargetTexture,
				NULL
			);

			if (FAILED(hr))
			{
				goto Exit;
			}

			backTargetTexture->GetSurfaceLevel(0, &backTarget);
		}

		d3d_device->SetRenderTarget(0, backTarget);
		d3d_device->SetDepthStencilSurface(nullptr);

		d3d_device->StretchRect(
			tempRender,
			nullptr,
			backTarget,
			nullptr,
			D3DTEXF_POINT);

		d3d_device->SetRenderTarget(0, tempRender);
		d3d_device->SetDepthStencilSurface(tempDepth);

		ret = true;

	Exit:;
		ES_SAFE_RELEASE(tempRender);
		ES_SAFE_RELEASE(tempDepth);
		*/
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

	void GraphicsGL::SetRenderTarget(RenderTextureGL* renderTexture, DepthTextureGL* depthTexture)
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

			cb[0] = renderTexture->GetBuffer();

			if (depthTexture != nullptr)
			{
				db = depthTexture->GetBuffer();
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
		/*
		HRESULT hr;

		hr = d3d_device->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &recordingTargetTexture, NULL);
		if (FAILED(hr)) return;

		d3d_device->GetRenderTarget(0, &renderDefaultTarget);
		d3d_device->GetDepthStencilSurface(&renderDefaultDepth);

		recordingTargetTexture->GetSurfaceLevel(0, &recordingTarget);

		d3d_device->CreateDepthStencilSurface(width, height, D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, TRUE, &recordingDepth, NULL);

		recordingWidth = width;
		recordingHeight = height;

		d3d_device->SetRenderTarget(0, recordingTarget);
		d3d_device->SetDepthStencilSurface(recordingDepth);
		*/
	}

	void GraphicsGL::EndRecord(std::vector<Effekseer::Color>& pixels)
	{
		/*
		pixels.resize(recordingWidth * recordingHeight);

		d3d_device->SetRenderTarget(0, renderDefaultTarget);
		d3d_device->SetDepthStencilSurface(renderDefaultDepth);
		ES_SAFE_RELEASE(renderDefaultTarget);
		ES_SAFE_RELEASE(renderDefaultDepth);

		IDirect3DSurface9*	temp_sur = nullptr;
		
		d3d_device->CreateOffscreenPlainSurface(
			recordingWidth, recordingHeight, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &temp_sur, NULL);
		
		d3d_device->GetRenderTargetData(recordingTarget, temp_sur);

		D3DLOCKED_RECT drect;
		RECT rect;
		rect.top = 0;
		rect.left = 0;
		rect.right = recordingWidth;
		rect.bottom = recordingHeight;

		auto hr = temp_sur->LockRect(&drect, &rect, D3DLOCK_READONLY);
		if (SUCCEEDED(hr))
		{
			for (int32_t y = 0; y < recordingHeight; y++)
			{
				for (int32_t x = 0; x < recordingWidth; x++)
				{
					auto src = &(((uint8_t*)drect.pBits)[x * 4 + drect.Pitch * y]);
					pixels[x + recordingWidth * y].A = src[3];
					pixels[x + recordingWidth * y].R = src[2];
					pixels[x + recordingWidth * y].G = src[1];
					pixels[x + recordingWidth * y].B = src[0];
				}
			}

			temp_sur->UnlockRect();
		}

		ES_SAFE_RELEASE(temp_sur);

		ES_SAFE_RELEASE(recordingTarget);
		ES_SAFE_RELEASE(recordingTargetTexture);
		ES_SAFE_RELEASE(recordingDepth);
		*/
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
		return nullptr;
		//return backTargetTexture;
	}

	EffekseerRenderer::Renderer* GraphicsGL::GetRenderer()
	{
		return renderer;
	}
}