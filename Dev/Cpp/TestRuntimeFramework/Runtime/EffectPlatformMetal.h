#pragma once

#include "EffectPlatformLLGI.h"

class EffectPlatformMetal final : public EffectPlatformLLGI
{
	LLGI::Texture* backgroundTexture_ = nullptr;

protected:
	void CreateShaders() override;
	EffekseerRenderer::RendererRef CreateRenderer() override;
	Effekseer::Backend::TextureRef CreateEffekseerTexture(LLGI::Texture* texture) override;

public:
	EffectPlatformMetal()
		: EffectPlatformLLGI(LLGI::DeviceType::Metal)
	{
	}

	virtual ~EffectPlatformMetal();

	void InitializeDevice(const EffectPlatformInitializingParameter& param) override;
	void DestroyDevice() override;

	LLGI::Texture* GetBackgroundTexture();
	void UpdateBackgroundTextureForDistortion();
	LLGI::Texture* GetCheckedTexture() const;
};
