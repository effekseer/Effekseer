#pragma once

#include "../../EffekseerRendererDX12/EffekseerRendererDX12.h"
#include "EffectPlatformLLGI.h"

class EffectPlatformDX12 final : public EffectPlatformLLGI
{
private:
	void CreateCheckedTexture();

	EffekseerRenderer::CommandList* commandListEfk_ = nullptr;
	EffekseerRenderer::SingleFrameMemoryPool* sfMemoryPoolEfk_ = nullptr;

	LLGI::RenderPass* renderPass_ = nullptr;
	LLGI::Texture* colorBuffer_ = nullptr;
	LLGI::Texture* depthBuffer_ = nullptr;
	LLGI::Shader* shader_vs_ = nullptr;
	LLGI::Shader* shader_ps_ = nullptr;
	LLGI::VertexBuffer* vb_ = nullptr;
	LLGI::IndexBuffer* ib_ = nullptr;
	LLGI::PipelineState* pip_ = nullptr;
	LLGI::RenderPassPipelineState* rppip_ = nullptr;
	LLGI::Texture* checkTexture_ = nullptr;

protected:
	EffekseerRenderer::Renderer* CreateRenderer() override;

public:
	EffectPlatformDX12() : EffectPlatformLLGI() {}

	virtual ~EffectPlatformDX12();

	void InitializeDevice(const EffectPlatformInitializingParameter& param) override;
	void DestroyDevice() override;
	void BeginRendering() override;
	void EndRendering() override;

	LLGI::Texture* GetCheckedTexture() const;
};