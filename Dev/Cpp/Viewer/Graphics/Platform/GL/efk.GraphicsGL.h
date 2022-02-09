
#pragma once

#include <Effekseer.h>
#include <EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.Renderer.h>
#include <EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.RendererImplemented.h>
#include <EffekseerRendererGL/EffekseerRenderer/GraphicsDevice.h>

#include "../../efk.Graphics.h"

namespace efk
{

class GraphicsGL : public Graphics
{
private:
	void* windowHandle = nullptr;
	int32_t windowWidth = 0;
	int32_t windowHeight = 0;
	GLuint frameBuffer = 0;

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

	void SetRenderTarget(std::vector<Effekseer::Backend::TextureRef> renderTextures, Effekseer::Backend::TextureRef depthTexture) override;

	void SaveTexture(Effekseer::Backend::TextureRef texture, std::vector<Effekseer::Color>& pixels) override;

	void Clear(Effekseer::Color color) override;

	void ResolveRenderTarget(Effekseer::Backend::TextureRef src, Effekseer::Backend::TextureRef dest) override;

	void ResetDevice() override;

	Effekseer::Tool::DeviceType GetDeviceType() const override
	{
		return Effekseer::Tool::DeviceType::OpenGL;
	}

	Effekseer::RefPtr<Effekseer::Backend::GraphicsDevice> GetGraphicsDevice() override
	{
		return graphicsDevice_;
	}
};
} // namespace efk