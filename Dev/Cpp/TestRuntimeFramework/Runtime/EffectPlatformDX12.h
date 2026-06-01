#pragma once

#include "../../EffekseerRendererDX12/EffekseerRendererDX12.h"
#include "EffectPlatformLLGI.h"

class EffectPlatformDX12 final : public EffectPlatformLLGI
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
	void BeginCompute() override;
	void EndCompute() override;
	void BeginRendering() override;
	void EndRendering() override;

	LLGI::Texture* GetBackgroundTexture();
	void UpdateBackgroundTextureForDistortion();
};
