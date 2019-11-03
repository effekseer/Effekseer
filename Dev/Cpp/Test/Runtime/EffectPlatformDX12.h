#pragma once

#include "../../EffekseerRendererDX12/EffekseerRendererDX12.h"
#include "EffectPlatformLLGI.h"


class EffectPlatformDX12 final : public EffectPlatformLLGI
{
private:
	void CreateCheckedTexture();

	EffekseerRenderer::CommandList* commandListEfk_ = nullptr;
	EffekseerRenderer::SingleFrameMemoryPool* sfMemoryPoolEfk_ = nullptr;

protected:
	EffekseerRenderer::Renderer* CreateRenderer() override;

public:
	EffectPlatformDX12() : EffectPlatformLLGI() {}

	~EffectPlatformDX12();

	void InitializeDevice(const EffectPlatformInitializingParameter& param) override;
	void DestroyDevice() override;
	void BeginRendering() override;
	void EndRendering() override;
	bool TakeScreenshot(const char* path) override;
};