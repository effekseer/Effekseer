#pragma once

#include "../../EffekseerRendererGL/EffekseerRendererGL.h"
#include "EffectPlatform.h"

class EffectPlatformGL final : public EffectPlatform
{
private:
protected:
	EffekseerRenderer::Renderer* CreateRenderer() override;

public:
	EffectPlatformGL() : EffectPlatform(true) {}

	~EffectPlatformGL();

	void InitializeDevice(const EffectPlatformInitializingParameter& param) override;
	void BeginRendering() override;
	void EndRendering() override;
	void Present() override;
	bool TakeScreenshot(const char* path) override;
};