#pragma once

#include "../../EffekseerRendererVulkan/EffekseerRendererVulkan.h"
#include "EffectPlatformLLGI.h"

class EffectPlatformVulkan final : public EffectPlatformLLGI
{
	LLGI::Texture* backgroundTexture_ = nullptr;

protected:
	void CreateShaders() override;
	EffekseerRenderer::RendererRef CreateRenderer() override;
	Effekseer::Backend::TextureRef CreateEffekseerTexture(LLGI::Texture* texture) override;

public:
	EffectPlatformVulkan()
		: EffectPlatformLLGI(LLGI::DeviceType::Vulkan)
	{
	}

	virtual ~EffectPlatformVulkan();

	void InitializeDevice(const EffectPlatformInitializingParameter& param) override;
	void DestroyDevice() override;

	LLGI::Texture* GetBackgroundTexture();
	void UpdateBackgroundTextureForDistortion();
};
