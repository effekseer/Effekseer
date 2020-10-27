#pragma once

#include "../../EffekseerRendererGL/EffekseerRendererGL.h"
#include "EffectPlatformGLFW.h"

class EffectPlatformGL final : public EffectPlatformGLFW
{
private:
protected:
	EffekseerRenderer::Renderer* CreateRenderer() override;

public:
	EffectPlatformGL()
		: EffectPlatformGLFW(true)
	{
	}

	~EffectPlatformGL();

	void InitializeDevice(const EffectPlatformInitializingParameter& param) override;
	void BeginRendering() override;
	void EndRendering() override;
	bool TakeScreenshot(const char* path) override;
};