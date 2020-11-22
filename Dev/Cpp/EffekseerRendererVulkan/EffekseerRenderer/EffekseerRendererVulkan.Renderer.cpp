#include "EffekseerRendererVulkan.Renderer.h"
#include "../../3rdParty/LLGI/src/Vulkan/LLGI.CommandListVulkan.h"
#include "../../3rdParty/LLGI/src/Vulkan/LLGI.GraphicsVulkan.h"
#include "../EffekseerRendererLLGI/EffekseerRendererLLGI.RendererImplemented.h"
#include "EffekseerMaterialCompilerVulkan.h"

#include "ShaderHeader/ad_sprite_distortion_ps.h"
#include "ShaderHeader/ad_sprite_distortion_vs.h"
#include "ShaderHeader/ad_sprite_lit_ps.h"
#include "ShaderHeader/ad_sprite_lit_vs.h"
#include "ShaderHeader/ad_sprite_unlit_ps.h"
#include "ShaderHeader/ad_sprite_unlit_vs.h"

#include "ShaderHeader/ad_model_distortion_ps.h"
#include "ShaderHeader/ad_model_distortion_vs.h"
#include "ShaderHeader/ad_model_lit_ps.h"
#include "ShaderHeader/ad_model_lit_vs.h"
#include "ShaderHeader/ad_model_unlit_ps.h"
#include "ShaderHeader/ad_model_unlit_vs.h"

#include "ShaderHeader/sprite_distortion_ps.h"
#include "ShaderHeader/sprite_distortion_vs.h"
#include "ShaderHeader/sprite_lit_ps.h"
#include "ShaderHeader/sprite_lit_vs.h"
#include "ShaderHeader/sprite_unlit_ps.h"
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
	shader->AdvancedSpriteUnlit_PS = GENERATE_VIEW(ad_sprite_unlit_ps);
	shader->AdvancedSpriteLit_VS = GENERATE_VIEW(ad_sprite_lit_vs);
	shader->AdvancedSpriteLit_PS = GENERATE_VIEW(ad_sprite_lit_ps);
	shader->AdvancedSpriteDistortion_VS = GENERATE_VIEW(ad_sprite_distortion_vs);
	shader->AdvancedSpriteDistortion_PS = GENERATE_VIEW(ad_sprite_distortion_ps);

	shader->AdvancedModelUnlit_VS = GENERATE_VIEW(ad_model_unlit_vs);
	shader->AdvancedModelUnlit_PS = GENERATE_VIEW(ad_model_unlit_ps);
	shader->AdvancedModelLit_VS = GENERATE_VIEW(ad_model_lit_vs);
	shader->AdvancedModelLit_PS = GENERATE_VIEW(ad_model_lit_ps);
	shader->AdvancedModelDistortion_VS = GENERATE_VIEW(ad_model_distortion_vs);
	shader->AdvancedModelDistortion_PS = GENERATE_VIEW(ad_model_distortion_ps);

	shader->SpriteUnlit_VS = GENERATE_VIEW(sprite_unlit_vs);
	shader->SpriteUnlit_PS = GENERATE_VIEW(sprite_unlit_ps);
	shader->SpriteLit_VS = GENERATE_VIEW(sprite_lit_vs);
	shader->SpriteLit_PS = GENERATE_VIEW(sprite_lit_ps);
	shader->SpriteDistortion_VS = GENERATE_VIEW(sprite_distortion_vs);
	shader->SpriteDistortion_PS = GENERATE_VIEW(sprite_distortion_ps);

	shader->ModelUnlit_VS = GENERATE_VIEW(model_unlit_vs);
	shader->ModelUnlit_PS = GENERATE_VIEW(model_unlit_ps);
	shader->ModelLit_VS = GENERATE_VIEW(model_lit_vs);
	shader->ModelLit_PS = GENERATE_VIEW(model_lit_ps);
	shader->ModelDistortion_VS = GENERATE_VIEW(model_distortion_vs);
	shader->ModelDistortion_PS = GENERATE_VIEW(model_distortion_ps);
}

::Effekseer::Backend::GraphicsDevice* CreateGraphicsDevice(
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

	auto ret = new EffekseerRendererLLGI::Backend::GraphicsDevice(graphics);
	ES_SAFE_RELEASE(graphics);
	return ret;
}

::EffekseerRenderer::Renderer*
Create(::Effekseer::Backend::GraphicsDevice* graphicsDevice, RenderPassInformation renderPassInformation, int32_t squareMaxCount)
{
	auto gd = static_cast<EffekseerRendererLLGI::Backend::GraphicsDevice*>(graphicsDevice);

	::EffekseerRendererLLGI::RendererImplemented* renderer = new ::EffekseerRendererLLGI::RendererImplemented(squareMaxCount);
	CreateFixedShaderForVulkan(&renderer->fixedShader_);

	LLGI::RenderPassPipelineStateKey key;
	key.RenderTargetFormats.resize(renderPassInformation.RenderTextureCount);
	key.IsPresent = renderPassInformation.DoesPresentToScreen;
	for (size_t i = 0; i < key.RenderTargetFormats.size(); i++)
	{
		key.RenderTargetFormats.at(i) = LLGI::VulkanHelper::VkFormatToTextureFormat(renderPassInformation.RenderTextureFormats.at(i));
	}

	key.DepthFormat = LLGI::VulkanHelper::VkFormatToTextureFormat(renderPassInformation.DepthFormat);

	auto pipelineState = gd->GetGraphics()->CreateRenderPassPipelineState(key);

	if (!renderer->Initialize(gd, pipelineState, false))
	{
		ES_SAFE_RELEASE(renderer);
		ES_SAFE_RELEASE(pipelineState);
		return nullptr;
	}

	ES_SAFE_RELEASE(pipelineState);

	renderer->platformType_ = Effekseer::CompiledMaterialPlatformType::Vulkan;
	renderer->materialCompiler_ = new Effekseer::MaterialCompilerVulkan();

	return renderer;
}

::EffekseerRenderer::Renderer* Create(VkPhysicalDevice physicalDevice,
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
		ES_SAFE_RELEASE(graphicDevice);
		return ret;
	}

	ES_SAFE_RELEASE(graphicDevice);
	return nullptr;
}

Effekseer::TextureData* CreateTextureData(::EffekseerRenderer::Renderer* renderer, const VulkanImageInfo& info)
{
	auto r = static_cast<::EffekseerRendererLLGI::RendererImplemented*>(renderer);
	return CreateTextureData(r->GetGraphicsDevice(), info);
}

Effekseer::TextureData* CreateTextureData(::Effekseer::Backend::GraphicsDevice* graphicsDevice, const VulkanImageInfo& info)
{
	LLGI::VulkanImageInfo llgiinfo;
	llgiinfo.image = info.image;
	llgiinfo.format = info.format;
	llgiinfo.aspect = info.aspect;

	auto g = static_cast<::EffekseerRendererLLGI::Backend::GraphicsDevice*>(graphicsDevice);
	auto texture_ = g->GetGraphics()->CreateTexture((uint64_t)(&llgiinfo));

	auto textureData = new Effekseer::TextureData();
	textureData->UserPtr = texture_;
	textureData->UserID = 0;
	textureData->TextureFormat = Effekseer::TextureFormatType::ABGR8;
	textureData->Width = 0;
	textureData->Height = 0;
	return textureData;
}

void DeleteTextureData(::EffekseerRenderer::Renderer* renderer, Effekseer::TextureData* textureData)
{
	auto r = static_cast<::EffekseerRendererLLGI::RendererImplemented*>(renderer);
	DeleteTextureData(r->GetGraphicsDevice(), textureData);
}

void DeleteTextureData(::Effekseer::Backend::GraphicsDevice* graphicsDevice, Effekseer::TextureData* textureData)
{
	auto texture = (LLGI::Texture*)textureData->UserPtr;
	texture->Release();
	delete textureData;
}

void FlushAndWait(::EffekseerRenderer::Renderer* renderer)
{
	auto r = static_cast<::EffekseerRendererLLGI::RendererImplemented*>(renderer);
	FlushAndWait(r->GetGraphicsDevice());
}

void FlushAndWait(::Effekseer::Backend::GraphicsDevice* graphicsDevice)
{
	auto gd = static_cast<::EffekseerRendererLLGI::Backend::GraphicsDevice*>(graphicsDevice);
	auto g = static_cast<LLGI::GraphicsVulkan*>(gd->GetGraphics());
	g->WaitFinish();
}

EffekseerRenderer::CommandList* CreateCommandList(::EffekseerRenderer::Renderer* renderer,
												  ::EffekseerRenderer::SingleFrameMemoryPool* memoryPool)
{
	auto r = static_cast<::EffekseerRendererLLGI::RendererImplemented*>(renderer);
	return CreateCommandList(r->GetGraphicsDevice(), memoryPool);
}

EffekseerRenderer::CommandList* CreateCommandList(::Effekseer::Backend::GraphicsDevice* graphicsDevice,
												  ::EffekseerRenderer::SingleFrameMemoryPool* memoryPool)
{
	auto gd = static_cast<::EffekseerRendererLLGI::Backend::GraphicsDevice*>(graphicsDevice);
	auto g = static_cast<LLGI::GraphicsVulkan*>(gd->GetGraphics());
	auto mp = static_cast<::EffekseerRendererLLGI::SingleFrameMemoryPool*>(memoryPool);
	auto commandList = g->CreateCommandList(mp->GetInternal());
	auto ret = new EffekseerRendererLLGI::CommandList(g, commandList, mp->GetInternal());
	ES_SAFE_RELEASE(commandList);
	return ret;
}

EffekseerRenderer::SingleFrameMemoryPool* CreateSingleFrameMemoryPool(::EffekseerRenderer::Renderer* renderer)
{
	auto r = static_cast<::EffekseerRendererLLGI::RendererImplemented*>(renderer);
	return CreateSingleFrameMemoryPool(r->GetGraphicsDevice());
}

EffekseerRenderer::SingleFrameMemoryPool* CreateSingleFrameMemoryPool(::Effekseer::Backend::GraphicsDevice* graphicsDevice)
{
	auto gd = static_cast<::EffekseerRendererLLGI::Backend::GraphicsDevice*>(graphicsDevice);
	auto g = static_cast<LLGI::GraphicsVulkan*>(gd->GetGraphics());
	auto mp = g->CreateSingleFrameMemoryPool(1024 * 1024 * 8, 128);
	auto ret = new EffekseerRendererLLGI::SingleFrameMemoryPool(mp);
	ES_SAFE_RELEASE(mp);
	return ret;
}

void BeginCommandList(EffekseerRenderer::CommandList* commandList, VkCommandBuffer nativeCommandList)
{
	assert(commandList != nullptr);

	auto c = static_cast<EffekseerRendererLLGI::CommandList*>(commandList);
	static_cast<LLGI::CommandListVulkan*>(c->GetInternal())->BeginExternal(nativeCommandList);
}

void EndCommandList(EffekseerRenderer::CommandList* commandList)
{
	assert(commandList != nullptr);
	auto c = static_cast<EffekseerRendererLLGI::CommandList*>(commandList);
	c->GetInternal()->EndWithPlatform();
}

} // namespace EffekseerRendererVulkan