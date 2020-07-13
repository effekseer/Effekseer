
#pragma once

#include <Effekseer.h>
#include <EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.Renderer.h>
#include <EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.RendererImplemented.h>

#include "../../efk.Graphics.h"

namespace efk
{
class RenderTextureGL : public RenderTexture
{
private:
	GLuint texture = 0;
	GLuint renderbuffer = 0;

public:
	RenderTextureGL(Graphics* graphics);
	virtual ~RenderTextureGL();
	bool Initialize(Effekseer::Tool::Vector2DI size, TextureFormat format, uint32_t multisample = 1);

	GLuint GetTexture()
	{
		return texture;
	}
	GLuint GetBuffer()
	{
		return renderbuffer;
	}

	uint64_t GetViewID() override
	{
		return texture;
	}
};

class DepthTextureGL : public DepthTexture
{
private:
	GLuint texture = 0;
	GLuint renderbuffer = 0;

public:
	DepthTextureGL(Graphics* graphics);
	virtual ~DepthTextureGL();
	bool Initialize(int32_t width, int32_t height, uint32_t multisample = 1);

	GLuint GetTexture()
	{
		return texture;
	}
	GLuint GetBuffer()
	{
		return renderbuffer;
	}
};

class GraphicsGL : public Graphics
{
private:
	void* windowHandle = nullptr;
	int32_t windowWidth = 0;
	int32_t windowHeight = 0;

	bool isSRGBMode = false;
	GLuint frameBuffer = 0;

	std::shared_ptr<RenderTextureGL> backTarget;
	GLuint frameBufferForCopySrc = 0;
	GLuint frameBufferForCopyDst = 0;

public:
	GraphicsGL();
	virtual ~GraphicsGL();

	bool Initialize(void* windowHandle, int32_t windowWidth, int32_t windowHeight, bool isSRGBMode) override;

	void CopyTo(RenderTexture* src, RenderTexture* dst) override;

	//void CopyToBackground() override;

	void Resize(int32_t width, int32_t height) override;

	bool Present() override;

	void BeginScene() override;

	void EndScene() override;

	void SetRenderTarget(RenderTexture* renderTexture, DepthTexture* depthTexture) override;

	//void BeginRecord(int32_t width, int32_t height) override;

	//void EndRecord(std::vector<Effekseer::Color>& pixels) override;

	void SaveTexture(RenderTexture* texture, std::vector<Effekseer::Color>& pixels) override;

	void Clear(Effekseer::Color color) override;

	void ResolveRenderTarget(RenderTexture* src, RenderTexture* dest) override;

	void ResetDevice() override;

	//void* GetBack() override;

	DeviceType GetDeviceType() const override
	{
		return DeviceType::OpenGL;
	}
};
} // namespace efk