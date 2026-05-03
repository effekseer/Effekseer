#pragma once

#include "../../EffekseerRendererWebGPU/EffekseerRendererWebGPU.h"
#include "EffectPlatformLLGI.h"

class EffectPlatformWebGPU final : public EffectPlatformLLGI
{
	LLGI::Texture* backgroundTexture_ = nullptr;

protected:
	void CreateShaders() override;
	EffekseerRenderer::RendererRef CreateRenderer() override;

public:
	EffectPlatformWebGPU()
		: EffectPlatformLLGI(LLGI::DeviceType::WebGPU)
	{
	}

	virtual ~EffectPlatformWebGPU();

	void InitializeDevice(const EffectPlatformInitializingParameter& param) override;
	void DestroyDevice() override;
	void BeginRendering() override;
	void EndRendering() override;

	LLGI::Texture* GetBackgroundTexture();
	void UpdateBackgroundTextureForDistortion();
};
