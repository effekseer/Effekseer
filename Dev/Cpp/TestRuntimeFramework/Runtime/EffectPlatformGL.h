#pragma once

#include "../../EffekseerRendererGL/EffekseerRendererGL.h"
#include "EffectPlatformGLFW.h"

class EffectPlatformGL final : public EffectPlatformGLFW
{
private:
	Effekseer::Backend::GraphicsDeviceRef graphicsDevice_ = nullptr;
	Effekseer::Backend::TextureRef checkedTexture_ = nullptr;
	GLuint frameBuffer_ = 0;

protected:
	EffekseerRenderer::RendererRef CreateRenderer() override;

public:
	EffectPlatformGL()
		: EffectPlatformGLFW(true)
	{
		isBackgroundFlipped_ = true;
	}

	~EffectPlatformGL();

	void InitializeDevice(const EffectPlatformInitializingParameter& param) override;
	void BeginRendering() override;
	void EndRendering() override;
	bool TakeScreenshot(const char* path) override;
};