#include "EffekseerRendererVulkan.Renderer.h"
#include "../../3rdParty/LLGI/src/Vulkan/LLGI.CommandListVulkan.h"
#include "../../3rdParty/LLGI/src/Vulkan/LLGI.GraphicsVulkan.h"
#include "../EffekseerRendererLLGI/EffekseerRendererLLGI.RendererImplemented.h"
#include "EffekseerMaterialCompilerVulkan.h"

#include "ShaderHeader/ad_model_distortion_ps.h"
#include "ShaderHeader/ad_sprite_distortion_vs.h"
#include "ShaderHeader/ad_sprite_lit_vs.h"
#include "ShaderHeader/ad_sprite_unlit_vs.h"

#include "ShaderHeader/ad_model_distortion_ps.h"
#include "ShaderHeader/ad_model_distortion_vs.h"
#include "ShaderHeader/ad_model_lit_ps.h"
#include "ShaderHeader/ad_model_lit_vs.h"
#include "ShaderHeader/ad_model_unlit_ps.h"
#include "ShaderHeader/ad_model_unlit_vs.h"

#include "ShaderHeader/model_distortion_ps.h"
#include "ShaderHeader/sprite_distortion_vs.h"
#include "ShaderHeader/sprite_lit_vs.h"
#include "ShaderHeader/sprite_unlit_vs.h"

#include "ShaderHeader/model_distortion_ps.h"
#include "ShaderHeader/model_distortion_vs.h"
#include "ShaderHeader/model_lit_ps.h"
#include "ShaderHeader/model_lit_vs.h"
#include "ShaderHeader/model_unlit_ps.h"
#include "ShaderHeader/model_unlit_vs.h"

#define GENERATE_VIEW(x) {{x, static_cast<int32_t>(sizeof(x))}};

namespace EffekseerRendererVulkan
{

static void CreateFixedShaderForVulkan(EffekseerRendererLLGI::FixedShader* shader)
{
	assert(shader);
	if (!shader)
		return;

	shader->AdvancedSpriteUnlit_VS = GENERATE_VIEW(ad_sprite_unlit_vs);
	shader->AdvancedSpriteLit_VS = GENERATE_VIEW(ad_sprite_lit_vs);
	shader->AdvancedSpriteDistortion_VS = GENERATE_VIEW(ad_sprite_distortion_vs);

	shader->AdvancedModelUnlit_VS = GENERATE_VIEW(ad_model_unlit_vs);
	shader->AdvancedModelUnlit_PS = GENERATE_VIEW(ad_model_unlit_ps);
	shader->AdvancedModelLit_VS = GENERATE_VIEW(ad_model_lit_vs);
	shader->AdvancedModelLit_PS = GENERATE_VIEW(ad_model_lit_ps);
	shader->AdvancedModelDistortion_VS = GENERATE_VIEW(ad_model_distortion_vs);
	shader->AdvancedModelDistortion_PS = GENERATE_VIEW(ad_model_distortion_ps);

	shader->SpriteUnlit_VS = GENERATE_VIEW(sprite_unlit_vs);
	shader->SpriteLit_VS = GENERATE_VIEW(sprite_lit_vs);
	shader->SpriteDistortion_VS = GENERATE_VIEW(sprite_distortion_vs);

	shader->ModelUnlit_VS = GENERATE_VIEW(model_unlit_vs);
	shader->ModelUnlit_PS = GENERATE_VIEW(model_unlit_ps);
	shader->ModelLit_VS = GENERATE_VIEW(model_lit_vs);
	shader->ModelLit_PS = GENERATE_VIEW(model_lit_ps);
	shader->ModelDistortion_VS = GENERATE_VIEW(model_distortion_vs);
	shader->ModelDistortion_PS = GENERATE_VIEW(model_distortion_ps);
}

::Effekseer::Backend::GraphicsDeviceRef CreateGraphicsDevice(
	VkPhysicalDevice physicalDevice, VkDevice device, VkQueue transfarQueue, VkCommandPool transfarCommandPool, int32_t swapBufferCount)
{
	auto graphics = new LLGI::GraphicsVulkan(
		vk::Device(device),
		vk::Queue(transfarQueue),
		vk::CommandPool(transfarCommandPool),
		vk::PhysicalDevice(physicalDevice),
		swapBufferCount,
		[](vk::CommandBuffer, vk::Fence) -> void {},
		nullptr,
		nullptr);

	auto ret = Effekseer::MakeRefPtr<EffekseerRendererLLGI::Backend::GraphicsDevice>(graphics);
	ES_SAFE_RELEASE(graphics);
	return ret;
}

::EffekseerRenderer::RendererRef
Create(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice, RenderPassInformation renderPassInformation, int32_t squareMaxCount)
{
	auto gd = graphicsDevice.DownCast<EffekseerRendererLLGI::Backend::GraphicsDevice>();

	auto renderer = Effekseer::MakeRefPtr<::EffekseerRendererLLGI::RendererImplemented>(squareMaxCount);
	CreateFixedShaderForVulkan(&renderer->fixedShader_);

	LLGI::RenderPassPipelineStateKey key;
	key.RenderTargetFormats.resize(renderPassInformation.RenderTextureCount);
	key.IsPresent = renderPassInformation.DoesPresentToScreen;
	for (size_t i = 0; i < key.RenderTargetFormats.size(); i++)
	{
		key.RenderTargetFormats.at(i) = LLGI::VulkanHelper::VkFormatToTextureFormat(renderPassInformation.RenderTextureFormats.at(i));
	}

	key.DepthFormat = LLGI::VulkanHelper::VkFormatToTextureFormat(renderPassInformation.DepthFormat);

	if (!renderer->Initialize(gd, key, false))
	{
		return nullptr;
	}

	renderer->platformType_ = Effekseer::CompiledMaterialPlatformType::Vulkan;
	renderer->materialCompiler_ = new Effekseer::MaterialCompilerVulkan();

	return renderer;
}

::EffekseerRenderer::RendererRef Create(VkPhysicalDevice physicalDevice,
										VkDevice device,
										VkQueue transfarQueue,
										VkCommandPool transfarCommandPool,
										int32_t swapBufferCount,
										RenderPassInformation renderPassInformation,
										int32_t squareMaxCount)
{
	auto graphicDevice = CreateGraphicsDevice(physicalDevice, device, transfarQueue, transfarCommandPool, swapBufferCount);

	auto ret = Create(graphicDevice, renderPassInformation, squareMaxCount);

	if (ret != nullptr)
	{
		return ret;
	}

	return nullptr;
}

Effekseer::Backend::TextureRef CreateTexture(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice, const VulkanImageInfo& info)
{
	LLGI::VulkanImageInfo llgiinfo;
	llgiinfo.image = info.image;
	llgiinfo.format = info.format;
	llgiinfo.aspect = info.aspect;

	auto g = static_cast<::EffekseerRendererLLGI::Backend::GraphicsDevice*>(graphicsDevice.Get());
	return g->CreateTexture((uint64_t)(&llgiinfo), [] {});
}

void BeginCommandList(Effekseer::RefPtr<EffekseerRenderer::CommandList> commandList, VkCommandBuffer nativeCommandList)
{
	assert(commandList != nullptr);

	auto c = static_cast<EffekseerRendererLLGI::CommandList*>(commandList.Get());

	LLGI::PlatformContextVulkan context;
	context.commandBuffer = nativeCommandList;

	static_cast<LLGI::CommandListVulkan*>(c->GetInternal())->BeginWithPlatform(&context);
	c->GetInternal()->BeginRenderPassWithPlatformPtr(nullptr);
}

void EndCommandList(Effekseer::RefPtr<EffekseerRenderer::CommandList> commandList)
{
	assert(commandList != nullptr);
	auto c = static_cast<EffekseerRendererLLGI::CommandList*>(commandList.Get());
	c->GetInternal()->EndRenderPassWithPlatformPtr();
	c->GetInternal()->EndWithPlatform();
}

} // namespace EffekseerRendererVulkan