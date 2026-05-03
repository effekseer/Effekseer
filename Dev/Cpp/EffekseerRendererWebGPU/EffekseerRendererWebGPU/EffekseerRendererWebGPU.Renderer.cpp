#include "EffekseerRendererWebGPU.Renderer.h"
#include "../../3rdParty/LLGI/src/WebGPU/LLGI.GraphicsWebGPU.h"
#include <EffekseerRendererLLGI/EffekseerRendererLLGI.RendererImplemented.h>

#include "EffekseerMaterialCompilerWebGPU.h"
#include "ShaderHeader/ad_model_distortion_ps.h"
#include "ShaderHeader/ad_model_distortion_vs.h"
#include "ShaderHeader/ad_model_lit_ps.h"
#include "ShaderHeader/ad_model_lit_vs.h"
#include "ShaderHeader/ad_model_unlit_ps.h"
#include "ShaderHeader/ad_model_unlit_vs.h"
#include "ShaderHeader/ad_sprite_distortion_vs.h"
#include "ShaderHeader/ad_sprite_lit_vs.h"
#include "ShaderHeader/ad_sprite_unlit_vs.h"
#include "ShaderHeader/gpu_particles_clear_cs.h"
#include "ShaderHeader/gpu_particles_render_ps.h"
#include "ShaderHeader/gpu_particles_render_vs.h"
#include "ShaderHeader/gpu_particles_spawn_cs.h"
#include "ShaderHeader/gpu_particles_update_cs.h"
#include "ShaderHeader/model_distortion_ps.h"
#include "ShaderHeader/model_distortion_vs.h"
#include "ShaderHeader/model_lit_ps.h"
#include "ShaderHeader/model_lit_vs.h"
#include "ShaderHeader/model_unlit_ps.h"
#include "ShaderHeader/model_unlit_vs.h"
#include "ShaderHeader/sprite_distortion_vs.h"
#include "ShaderHeader/sprite_lit_vs.h"
#include "ShaderHeader/sprite_unlit_vs.h"

#define GENERATE_VIEW(x) {{x, static_cast<int32_t>(sizeof(x))}};

namespace EffekseerRendererWebGPU
{

static void CreateFixedShaderForWebGPU(EffekseerRendererLLGI::FixedShader* shader)
{
	assert(shader);
	if (!shader)
		return;

	shader->AdvancedSpriteUnlit_VS = GENERATE_VIEW(webgpu_ad_sprite_unlit_vs);
	shader->AdvancedSpriteLit_VS = GENERATE_VIEW(webgpu_ad_sprite_lit_vs);
	shader->AdvancedSpriteDistortion_VS = GENERATE_VIEW(webgpu_ad_sprite_distortion_vs);

	shader->AdvancedModelUnlit_VS = GENERATE_VIEW(webgpu_ad_model_unlit_vs);
	shader->AdvancedModelUnlit_PS = GENERATE_VIEW(webgpu_ad_model_unlit_ps);
	shader->AdvancedModelLit_VS = GENERATE_VIEW(webgpu_ad_model_lit_vs);
	shader->AdvancedModelLit_PS = GENERATE_VIEW(webgpu_ad_model_lit_ps);
	shader->AdvancedModelDistortion_VS = GENERATE_VIEW(webgpu_ad_model_distortion_vs);
	shader->AdvancedModelDistortion_PS = GENERATE_VIEW(webgpu_ad_model_distortion_ps);

	shader->SpriteUnlit_VS = GENERATE_VIEW(webgpu_sprite_unlit_vs);
	shader->SpriteLit_VS = GENERATE_VIEW(webgpu_sprite_lit_vs);
	shader->SpriteDistortion_VS = GENERATE_VIEW(webgpu_sprite_distortion_vs);

	shader->ModelUnlit_VS = GENERATE_VIEW(webgpu_model_unlit_vs);
	shader->ModelUnlit_PS = GENERATE_VIEW(webgpu_model_unlit_ps);
	shader->ModelLit_VS = GENERATE_VIEW(webgpu_model_lit_vs);
	shader->ModelLit_PS = GENERATE_VIEW(webgpu_model_lit_ps);
	shader->ModelDistortion_VS = GENERATE_VIEW(webgpu_model_distortion_vs);
	shader->ModelDistortion_PS = GENERATE_VIEW(webgpu_model_distortion_ps);

	shader->GpuParticles_Clear_CS = GENERATE_VIEW(webgpu_gpu_particles_clear_cs);
	shader->GpuParticles_Spawn_CS = GENERATE_VIEW(webgpu_gpu_particles_spawn_cs);
	shader->GpuParticles_Update_CS = GENERATE_VIEW(webgpu_gpu_particles_update_cs);
	shader->GpuParticles_Render_VS = GENERATE_VIEW(webgpu_gpu_particles_render_vs);
	shader->GpuParticles_Render_PS = GENERATE_VIEW(webgpu_gpu_particles_render_ps);
}

::Effekseer::Backend::GraphicsDeviceRef CreateGraphicsDevice(wgpu::Device device)
{
	auto graphics = new LLGI::GraphicsWebGPU(device);
	auto ret = Effekseer::MakeRefPtr<EffekseerRendererLLGI::Backend::GraphicsDevice>(graphics);
	ES_SAFE_RELEASE(graphics);
	return ret;
}

::Effekseer::Backend::GraphicsDeviceRef CreateGraphicsDevice(wgpu::Device device, wgpu::Instance instance)
{
	auto graphics = new LLGI::GraphicsWebGPU(device, instance);
	auto ret = Effekseer::MakeRefPtr<EffekseerRendererLLGI::Backend::GraphicsDevice>(graphics);
	ES_SAFE_RELEASE(graphics);
	return ret;
}

::EffekseerRenderer::RendererRef
Create(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice, RenderPassInformation renderPassInformation, int32_t squareMaxCount)
{
	auto gd = graphicsDevice.DownCast<EffekseerRendererLLGI::Backend::GraphicsDevice>();
	auto renderer = Effekseer::MakeRefPtr<::EffekseerRendererLLGI::RendererImplemented>(squareMaxCount);
	CreateFixedShaderForWebGPU(&renderer->fixedShader_);

	LLGI::RenderPassPipelineStateKey key;
	key.RenderTargetFormats.resize(renderPassInformation.RenderTextureCount);
	key.IsPresent = renderPassInformation.DoesPresentToScreen;
	for (size_t i = 0; i < key.RenderTargetFormats.size(); i++)
	{
		key.RenderTargetFormats.at(i) = LLGI::ConvertFormat(renderPassInformation.RenderTextureFormats.at(i));
	}
	key.DepthFormat = LLGI::ConvertFormat(renderPassInformation.DepthFormat);

	renderer->platformType_ = Effekseer::CompiledMaterialPlatformType::WebGPU;
	renderer->materialCompiler_ = new Effekseer::MaterialCompilerWebGPU();

	if (!renderer->Initialize(gd, key, false))
	{
		return nullptr;
	}

	return renderer;
}

::EffekseerRenderer::RendererRef Create(wgpu::Device device, RenderPassInformation renderPassInformation, int32_t squareMaxCount)
{
	auto graphicDevice = CreateGraphicsDevice(device);
	return Create(graphicDevice, renderPassInformation, squareMaxCount);
}

::EffekseerRenderer::RendererRef
Create(wgpu::Device device, wgpu::Instance instance, RenderPassInformation renderPassInformation, int32_t squareMaxCount)
{
	auto graphicDevice = CreateGraphicsDevice(device, instance);
	return Create(graphicDevice, renderPassInformation, squareMaxCount);
}

void BeginCommandList(Effekseer::RefPtr<EffekseerRenderer::CommandList> commandList)
{
	assert(commandList != nullptr);
	auto c = static_cast<EffekseerRendererLLGI::CommandList*>(commandList.Get());
	c->GetInternal()->Begin();
	c->SetState(EffekseerRendererLLGI::CommandListState::Running);
	c->GetInternal()->BeginComputePass();
}

void EndCommandList(Effekseer::RefPtr<EffekseerRenderer::CommandList> commandList)
{
	assert(commandList != nullptr);
	auto c = static_cast<EffekseerRendererLLGI::CommandList*>(commandList.Get());
	c->GetInternal()->EndComputePass();
	c->GetInternal()->End();
	c->SetState(EffekseerRendererLLGI::CommandListState::Wait);
}

void ExecuteCommandList(Effekseer::RefPtr<EffekseerRenderer::CommandList> commandList)
{
	assert(commandList != nullptr);
	auto c = static_cast<EffekseerRendererLLGI::CommandList*>(commandList.Get());
	c->GetGraphics()->Execute(c->GetInternal());
}

} // namespace EffekseerRendererWebGPU
