#pragma once

#include "EffectPlatformLLGI.h"

class EffectPlatformMetal final : public EffectPlatformLLGI
{
protected:
	void CreateShaders() override;
	EffekseerRenderer::RendererRef CreateRenderer() override;

public:
	EffectPlatformMetal() : EffectPlatformLLGI(LLGI::DeviceType::Metal) {}

	virtual ~EffectPlatformMetal();

	void InitializeDevice(const EffectPlatformInitializingParameter& param) override;
	void DestroyDevice() override;
	void BeginRendering() override;
	void EndRendering() override;

	LLGI::Texture* GetCheckedTexture() const;
};
