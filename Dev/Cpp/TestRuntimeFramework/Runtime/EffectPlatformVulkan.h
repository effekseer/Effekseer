#pragma once

#include "../../EffekseerRendererVulkan/EffekseerRendererVulkan.h"
#include "EffectPlatformLLGI.h"

class EffectPlatformVulkan final : public EffectPlatformLLGI
{
protected:
	void CreateShaders() override;
	EffekseerRenderer::RendererRef CreateRenderer() override;

public:
	EffectPlatformVulkan() : EffectPlatformLLGI(LLGI::DeviceType::Vulkan) {}

	virtual ~EffectPlatformVulkan();

	void InitializeDevice(const EffectPlatformInitializingParameter& param) override;
	void DestroyDevice() override;
	void BeginRendering() override;
	void EndRendering() override;

	LLGI::Texture* GetCheckedTexture() const;
};