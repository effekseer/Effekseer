#pragma once

#include "../../EffekseerRendererGL/EffekseerRendererGL.h"
#include "EffectPlatformGLFW.h"

class EffectPlatformGL final : public EffectPlatformGLFW
{
private:
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