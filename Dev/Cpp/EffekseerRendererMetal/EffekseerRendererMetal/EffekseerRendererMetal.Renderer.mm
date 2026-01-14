#include "EffekseerRendererMetal.Renderer.h"
#include "../../3rdParty/LLGI/src/Metal/LLGI.CommandListMetal.h"
#include "../../3rdParty/LLGI/src/Metal/LLGI.GraphicsMetal.h"
#include "../../3rdParty/LLGI/src/Metal/LLGI.Metal_Impl.h"
#include "../../3rdParty/LLGI/src/Metal/LLGI.RenderPassMetal.h"
#include <EffekseerRendererCommon/EffekseerRenderer.CommonUtils.h>
#include <EffekseerRendererLLGI/EffekseerRendererLLGI.MaterialLoader.h>
#include <EffekseerRendererLLGI/EffekseerRendererLLGI.Shader.h>

#include "../../EffekseerMaterialCompiler/Metal/EffekseerMaterialCompilerMetal.h"

// #include "Shaders.h"

#include "ShaderHeader/ad_sprite_distortion_vs.h"
#include "ShaderHeader/ad_sprite_lit_vs.h"
#include "ShaderHeader/ad_sprite_unlit_vs.h"

#include "ShaderHeader/ad_model_distortion_ps.h"
#include "ShaderHeader/ad_model_distortion_vs.h"
#include "ShaderHeader/ad_model_lit_ps.h"
#include "ShaderHeader/ad_model_lit_vs.h"
#include "ShaderHeader/ad_model_unlit_ps.h"
#include "ShaderHeader/ad_model_unlit_vs.h"

#include "ShaderHeader/sprite_distortion_vs.h"
#include "ShaderHeader/sprite_lit_vs.h"
#include "ShaderHeader/sprite_unlit_vs.h"

#include "ShaderHeader/model_distortion_ps.h"
#include "ShaderHeader/model_distortion_vs.h"
#include "ShaderHeader/model_lit_ps.h"
#include "ShaderHeader/model_lit_vs.h"
#include "ShaderHeader/model_unlit_ps.h"
#include "ShaderHeader/model_unlit_vs.h"

#include "ShaderHeader/gpu_particles_clear_cs.h"
#include "ShaderHeader/gpu_particles_render_ps.h"
#include "ShaderHeader/gpu_particles_render_vs.h"
#include "ShaderHeader/gpu_particles_spawn_cs.h"
#include "ShaderHeader/gpu_particles_update_cs.h"

#define GENERATE_VIEW(x) {{x, static_cast<int32_t>(sizeof(x))}};

namespace EffekseerRendererMetal
{

::Effekseer::MaterialLoaderRef CreateMaterialLoader(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice, ::Effekseer::FileInterfaceRef fileInterface)
{
	auto gd = graphicsDevice.DownCast<EffekseerRendererLLGI::Backend::GraphicsDevice>();
	auto compiler = new ::Effekseer::MaterialCompilerMetal();
	auto ret = ::Effekseer::MaterialLoaderRef(new ::EffekseerRendererLLGI::MaterialLoader(gd, fileInterface, ::Effekseer::CompiledMaterialPlatformType::Metal, compiler));
	ES_SAFE_RELEASE(compiler);
	return ret;
}

::Effekseer::Backend::GraphicsDeviceRef CreateDevice()
{
	auto graphics = new LLGI::GraphicsMetal();
	graphics->Initialize(nullptr);

	auto ret = Effekseer::MakeRefPtr<EffekseerRendererLLGI::Backend::GraphicsDevice>(graphics);
	ES_SAFE_RELEASE(graphics);
	return ret;
}

static void CreateFixedShaderForMetal(EffekseerRendererLLGI::FixedShader* shader)
{
	assert(shader);
	if (!shader)
		return;

	shader->AdvancedSpriteUnlit_VS = GENERATE_VIEW(metal_ad_sprite_unlit_vs);
	shader->AdvancedSpriteLit_VS = GENERATE_VIEW(metal_ad_sprite_lit_vs);
	shader->AdvancedSpriteDistortion_VS = GENERATE_VIEW(metal_ad_sprite_distortion_vs);

	shader->AdvancedModelUnlit_VS = GENERATE_VIEW(metal_ad_model_unlit_vs);
	shader->AdvancedModelUnlit_PS = GENERATE_VIEW(metal_ad_model_unlit_ps);
	shader->AdvancedModelLit_VS = GENERATE_VIEW(metal_ad_model_lit_vs);
	shader->AdvancedModelLit_PS = GENERATE_VIEW(metal_ad_model_lit_ps);
	shader->AdvancedModelDistortion_VS = GENERATE_VIEW(metal_ad_model_distortion_vs);
	shader->AdvancedModelDistortion_PS = GENERATE_VIEW(metal_ad_model_distortion_ps);

	shader->SpriteUnlit_VS = GENERATE_VIEW(metal_sprite_unlit_vs);
	shader->SpriteLit_VS = GENERATE_VIEW(metal_sprite_lit_vs);
	shader->SpriteDistortion_VS = GENERATE_VIEW(metal_sprite_distortion_vs);

	shader->ModelUnlit_VS = GENERATE_VIEW(metal_model_unlit_vs);
	shader->ModelUnlit_PS = GENERATE_VIEW(metal_model_unlit_ps);
	shader->ModelLit_VS = GENERATE_VIEW(metal_model_lit_vs);
	shader->ModelLit_PS = GENERATE_VIEW(metal_model_lit_ps);
	shader->ModelDistortion_VS = GENERATE_VIEW(metal_model_distortion_vs);
	shader->ModelDistortion_PS = GENERATE_VIEW(metal_model_distortion_ps);

	shader->GpuParticles_Clear_CS = GENERATE_VIEW(metal_gpu_particles_clear_cs);
	shader->GpuParticles_Spawn_CS = GENERATE_VIEW(metal_gpu_particles_spawn_cs);
	shader->GpuParticles_Update_CS = GENERATE_VIEW(metal_gpu_particles_update_cs);
	shader->GpuParticles_Render_VS = GENERATE_VIEW(metal_gpu_particles_render_vs);
	shader->GpuParticles_Render_PS = GENERATE_VIEW(metal_gpu_particles_render_ps);
}

::EffekseerRenderer::RendererRef Create(
	::Effekseer::Backend::GraphicsDeviceRef graphicsDevice,
	int32_t squareMaxCount,
	MTLPixelFormat renderTargetFormat,
	MTLPixelFormat depthStencilFormat,
	bool isReversedDepth)
{
	auto renderer = Effekseer::MakeRefPtr<::EffekseerRendererLLGI::RendererImplemented>(squareMaxCount);
	renderer->materialCompiler_ = new ::Effekseer::MaterialCompilerMetal();

	CreateFixedShaderForMetal(&renderer->fixedShader_);

	auto gd = graphicsDevice.DownCast<EffekseerRendererLLGI::Backend::GraphicsDevice>();
	auto g = static_cast<LLGI::GraphicsMetal*>(gd->GetGraphics());
	LLGI::RenderPassPipelineStateKey key;
	key.RenderTargetFormats.resize(1);
	key.RenderTargetFormats.at(0) = LLGI::ConvertFormat(renderTargetFormat);
	key.DepthFormat = LLGI::ConvertFormat(depthStencilFormat);
	key.IsColorCleared = false;
	key.IsDepthCleared = false;

	if (renderer->Initialize(gd, key, isReversedDepth))
	{
		return renderer;
	}

	return nullptr;
}

::EffekseerRenderer::RendererRef Create(int32_t squareMaxCount,
										MTLPixelFormat renderTargetFormat,
										MTLPixelFormat depthStencilFormat,
										bool isReversedDepth)
{
	auto graphicDevice = CreateDevice();

	auto ret = Create(graphicDevice, squareMaxCount, renderTargetFormat, depthStencilFormat, isReversedDepth);

	if (ret != nullptr)
	{
		return ret;
	}

	return nullptr;
}

Effekseer::Backend::TextureRef CreateTexture(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice, id<MTLTexture> texture)
{
	auto g = static_cast<::EffekseerRendererLLGI::Backend::GraphicsDevice*>(graphicsDevice.Get());
	return g->CreateTexture((uint64_t)texture, []() -> void {});
}

void BeginCommandList(Effekseer::RefPtr<EffekseerRenderer::CommandList> commandList)
{
	assert(commandList != nullptr);

	auto c = static_cast<EffekseerRendererLLGI::CommandList*>(commandList.Get());
	c->GetInternal()->BeginWithPlatform(nullptr);
}

void EndCommandList(Effekseer::RefPtr<EffekseerRenderer::CommandList> commandList)
{
	assert(commandList != nullptr);
	auto c = static_cast<EffekseerRendererLLGI::CommandList*>(commandList.Get());
	c->GetInternal()->EndWithPlatform();
}

void BeginRenderPass(Effekseer::RefPtr<EffekseerRenderer::CommandList> commandList, id<MTLRenderCommandEncoder> renderEncoder)
{
	assert(commandList != nullptr);

	LLGI::CommandListMetalPlatformRenderPassContext context;
	context.RenderEncoder = renderEncoder;

	auto c = static_cast<EffekseerRendererLLGI::CommandList*>(commandList.Get());
	c->GetInternal()->BeginRenderPassWithPlatformPtr(&context);
}

void EndRenderPass(Effekseer::RefPtr<EffekseerRenderer::CommandList> commandList)
{
	assert(commandList != nullptr);
	auto c = static_cast<EffekseerRendererLLGI::CommandList*>(commandList.Get());
	c->GetInternal()->EndRenderPassWithPlatformPtr();
}

void BeginComputePass(Effekseer::RefPtr<EffekseerRenderer::CommandList> commandList, id<MTLComputeCommandEncoder> computeEncoder)
{
	assert(commandList != nullptr);

	LLGI::CommandListMetalPlatformComputePassContext context;
	context.ComputeEncoder = computeEncoder;

	auto c = static_cast<EffekseerRendererLLGI::CommandList*>(commandList.Get());
	c->GetInternal()->BeginComputePassWithPlatformPtr(&context);
}

void EndComputePass(Effekseer::RefPtr<EffekseerRenderer::CommandList> commandList)
{
	assert(commandList != nullptr);
	auto c = static_cast<EffekseerRendererLLGI::CommandList*>(commandList.Get());
	c->GetInternal()->EndComputePassWithPlatformPtr();
}

} // namespace EffekseerRendererMetal
