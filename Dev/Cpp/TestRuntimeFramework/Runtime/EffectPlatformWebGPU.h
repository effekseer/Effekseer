#pragma once

#include "../../EffekseerRendererWebGPU/EffekseerRendererWebGPU.h"
#include "EffectPlatformLLGI.h"
#include <unordered_map>

class EffectPlatformWebGPU final : public EffectPlatformLLGI
{
	LLGI::Texture* backgroundTexture_ = nullptr;
	std::unordered_map<LLGI::CommandList*, Effekseer::RefPtr<EffekseerRenderer::CommandList>> commandListsEfk_;

	void BindCommandList(bool newRecording);

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
	void PreDestroyDevice() override;
	void DestroyDevice() override;
	void BeginCompute() override;
	void EndCompute() override;
	void BeginRendering() override;
	void EndRendering() override;

	LLGI::Texture* GetBackgroundTexture();
	void UpdateBackgroundTextureForDistortion();
};
