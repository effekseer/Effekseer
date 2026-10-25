#pragma once

#include "../../EffekseerRendererDX12/EffekseerRendererDX12.h"
#include "EffectPlatformLLGI.h"

// Tests may specialize frame setup to exercise resource exhaustion and multi-step compute.
class EffectPlatformDX12 : public EffectPlatformLLGI
{
	LLGI::Texture* backgroundTexture_ = nullptr;

protected:
	void CreateShaders() override;
	EffekseerRenderer::RendererRef CreateRenderer() override;
	Effekseer::Backend::TextureRef CreateEffekseerTexture(LLGI::Texture* texture) override;

public:
	EffectPlatformDX12()
		: EffectPlatformLLGI(LLGI::DeviceType::DirectX12)
	{
	}

	virtual ~EffectPlatformDX12();

	void InitializeDevice(const EffectPlatformInitializingParameter& param) override;
	void DestroyDevice() override;

	LLGI::Texture* GetBackgroundTexture();
	void UpdateBackgroundTextureForDistortion();
};
