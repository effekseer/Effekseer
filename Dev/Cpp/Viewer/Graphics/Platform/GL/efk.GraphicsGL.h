
#pragma once

#include <Effekseer.h>
#include <EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.Renderer.h>
#include <EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.RendererImplemented.h>
#include <EffekseerRendererGL/EffekseerRenderer/GraphicsDevice.h>

#include "../../efk.Graphics.h"

namespace efk
{
class RenderTextureGL : public RenderTexture
{
private:
	Graphics* graphics = nullptr;
	EffekseerRendererGL::Backend::TextureRef texture_;

public:
	RenderTextureGL(Graphics* graphics);
	virtual ~RenderTextureGL();
	bool Initialize(Effekseer::Tool::Vector2DI size, Effekseer::Backend::TextureFormatType format, uint32_t multisample = 1);

	Effekseer::Backend::TextureRef GetAsBackend() override
	{
		return texture_;
	}

	GLuint GetTexture()
	{
		return texture_->GetBuffer();
	}
	GLuint GetBuffer()
	{
		return texture_->GetRenderBuffer();
	}

	uint64_t GetViewID() override
	{
		return texture_->GetBuffer();
	}
};

class DepthTextureGL : public DepthTexture
{
private:
	Graphics* graphics_ = nullptr;
	EffekseerRendererGL::Backend::TextureRef texture_;

public:
	DepthTextureGL(Graphics* graphics);
	virtual ~DepthTextureGL();
	bool Initialize(int32_t width, int32_t height, uint32_t multisample = 1);

	Effekseer::Backend::TextureRef GetAsBackend() override
	{
		return texture_;
	}

	GLuint GetTexture()
	{
		return texture_->GetBuffer();
	}
	GLuint GetBuffer()
	{
		return texture_->GetRenderBuffer();
	}
};

class GraphicsGL : public Graphics
{
private:
	void* windowHandle = nullptr;
	int32_t windowWidth = 0;
	int32_t windowHeight = 0;
	GLuint frameBuffer = 0;

	std::shared_ptr<RenderTextureGL> backTarget;
	GLuint frameBufferForCopySrc = 0;
	GLuint frameBufferForCopyDst = 0;
	Effekseer::RefPtr<Effekseer::Backend::GraphicsDevice> graphicsDevice_;
	int32_t currentRenderTargetCount_ = 0;
	bool hasDepthBuffer_ = false;

public:
	GraphicsGL();
	virtual ~GraphicsGL();

	bool Initialize(void* windowHandle, int32_t windowWidth, int32_t windowHeight) override;

	void CopyTo(Effekseer::Backend::TextureRef src, Effekseer::Backend::TextureRef dst) override;

	void Resize(int32_t width, int32_t height) override;

	bool Present() override;

	void BeginScene() override;

	void EndScene() override;

	void SetRenderTarget(std::vector<Effekseer::Backend::TextureRef> renderTextures, Effekseer::Backend::TextureRef depthTexture) override;

	void SaveTexture(Effekseer::Backend::TextureRef texture, std::vector<Effekseer::Color>& pixels) override;

	void Clear(Effekseer::Color color) override;

	void ResolveRenderTarget(Effekseer::Backend::TextureRef src, Effekseer::Backend::TextureRef dest) override;

	void ResetDevice() override;

	DeviceType GetDeviceType() const override
	{
		return DeviceType::OpenGL;
	}

	Effekseer::RefPtr<Effekseer::Backend::GraphicsDevice> GetGraphicsDevice() override
	{
		return graphicsDevice_;
	}
};
} // namespace efk