#include "EffekseerRendererVulkan.Renderer.h"
#include "../../3rdParty/LLGI/src/Vulkan/LLGI.CommandListVulkan.h"
#include "../../3rdParty/LLGI/src/Vulkan/LLGI.GraphicsVulkan.h"
#include "../EffekseerRendererLLGI/EffekseerRendererLLGI.RendererImplemented.h"
#include "EffekseerMaterialCompilerVulkan.h"

#include "ShaderHeader/standard_renderer_PS.h"
#include "ShaderHeader/standard_renderer_VS.h"
#include "ShaderHeader/standard_renderer_distortion_PS.h"
#include "ShaderHeader/standard_renderer_distortion_VS.h"
#include "ShaderHeader/standard_renderer_lighting_PS.h"
#include "ShaderHeader/standard_renderer_lighting_VS.h"

#include "ShaderHeader/model_renderer_distortion_PS.h"
#include "ShaderHeader/model_renderer_distortion_VS.h"
#include "ShaderHeader/model_renderer_lighting_texture_normal_PS.h"
#include "ShaderHeader/model_renderer_lighting_texture_normal_VS.h"
#include "ShaderHeader/model_renderer_texture_PS.h"
#include "ShaderHeader/model_renderer_texture_VS.h"

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

namespace EffekseerRendererVulkan
{

static void CreateFixedShaderForVulkan(EffekseerRendererLLGI::FixedShader* shader)
{
	assert(shader);
	if (!shader)
		return;

	shader->AdvancedSpriteUnlit_VS = {{standard_renderer_VS, (int32_t)sizeof(standard_renderer_VS)}};
	shader->AdvancedSpriteLit_VS = {{standard_renderer_lighting_VS, (int32_t)sizeof(standard_renderer_lighting_VS)}};
	shader->AdvancedSpriteDistortion_VS = {{standard_renderer_distortion_VS, (int32_t)sizeof(standard_renderer_distortion_VS)}};
	shader->AdvancedModelUnlit_VS = {{model_renderer_texture_VS, (int32_t)sizeof(model_renderer_texture_VS)}};
	shader->AdvancedModelLit_VS = {{model_renderer_lighting_texture_normal_VS, (int32_t)sizeof(model_renderer_lighting_texture_normal_VS)}};
	shader->AdvancedModelDistortion_VS = {{model_renderer_distortion_VS, (int32_t)sizeof(model_renderer_distortion_VS)}};

	shader->AdvancedSpriteUnlit_PS = {{standard_renderer_PS, (int32_t)sizeof(standard_renderer_PS)}};
	shader->AdvancedSpriteLit_PS = {{standard_renderer_lighting_PS, (int32_t)sizeof(standard_renderer_lighting_PS)}};
	shader->AdvancedSpriteDistortion_PS = {{standard_renderer_distortion_PS, (int32_t)sizeof(standard_renderer_distortion_PS)}};
	shader->AdvancedModelUnlit_PS = {{model_renderer_texture_PS, (int32_t)sizeof(model_renderer_texture_PS)}};
	shader->AdvancedModelLit_PS = {{model_renderer_lighting_texture_normal_PS, (int32_t)sizeof(model_renderer_lighting_texture_normal_PS)}};
	shader->AdvancedModelDistortion_PS = {{model_renderer_distortion_PS, (int32_t)sizeof(model_renderer_distortion_PS)}};


	shader->SpriteUnlit_VS = {{sprite_unlit_vs, (int32_t)sizeof(sprite_unlit_vs)}};
	shader->SpriteDistortion_VS = {{sprite_distortion_vs, (int32_t)sizeof(sprite_distortion_vs)}};
	shader->SpriteLit_VS = {{sprite_lit_vs, (int32_t)sizeof(sprite_lit_vs)}};
	shader->ModelUnlit_VS = {{model_unlit_vs, (int32_t)sizeof(model_unlit_vs)}};
	shader->ModelDistortion_VS = {{model_distortion_vs, (int32_t)sizeof(model_distortion_vs)}};
	shader->ModelLit_VS = {{model_lit_vs, (int32_t)sizeof(model_lit_vs)}};

	shader->SpriteUnlit_PS = {{sprite_unlit_ps, (int32_t)sizeof(sprite_unlit_ps)}};
	shader->SpriteDistortion_PS = {{sprite_distortion_ps, (int32_t)sizeof(sprite_distortion_ps)}};
	shader->SpriteLit_PS = {{sprite_lit_ps, (int32_t)sizeof(sprite_lit_ps)}};
	shader->ModelUnlit_PS = {{model_unlit_ps, (int32_t)sizeof(model_unlit_ps)}};
	shader->ModelDistortion_PS = {{model_distortion_ps, (int32_t)sizeof(model_distortion_ps)}};
	shader->ModelLit_PS = {{model_lit_ps, (int32_t)sizeof(model_lit_ps)}};
}

::EffekseerRenderer::GraphicsDevice* CreateDevice(
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

	auto ret = new EffekseerRendererLLGI::GraphicsDevice(graphics);
	ES_SAFE_RELEASE(graphics);
	return ret;
}

::EffekseerRenderer::Renderer*
Create(::EffekseerRenderer::GraphicsDevice* graphicsDevice, RenderPassInformation renderPassInformation, int32_t squareMaxCount)
{
	auto gd = static_cast<EffekseerRendererLLGI::GraphicsDevice*>(graphicsDevice);

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
	auto graphicDevice = CreateDevice(physicalDevice, device, transfarQueue, transfarCommandPool, swapBufferCount);

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

Effekseer::TextureData* CreateTextureData(::EffekseerRenderer::GraphicsDevice* graphicsDevice, const VulkanImageInfo& info)
{
	LLGI::VulkanImageInfo llgiinfo;
	llgiinfo.image = info.image;
	llgiinfo.format = info.format;
	llgiinfo.aspect = info.aspect;

	auto g = static_cast<::EffekseerRendererLLGI::GraphicsDevice*>(graphicsDevice);
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

void DeleteTextureData(::EffekseerRenderer::GraphicsDevice* graphicsDevice, Effekseer::TextureData* textureData)
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

void FlushAndWait(::EffekseerRenderer::GraphicsDevice* graphicsDevice)
{
	auto gd = static_cast<::EffekseerRendererLLGI::GraphicsDevice*>(graphicsDevice);
	auto g = static_cast<LLGI::GraphicsVulkan*>(gd->GetGraphics());
	g->WaitFinish();
}

EffekseerRenderer::CommandList* CreateCommandList(::EffekseerRenderer::Renderer* renderer,
												  ::EffekseerRenderer::SingleFrameMemoryPool* memoryPool)
{
	auto r = static_cast<::EffekseerRendererLLGI::RendererImplemented*>(renderer);
	return CreateCommandList(r->GetGraphicsDevice(), memoryPool);
}

EffekseerRenderer::CommandList* CreateCommandList(::EffekseerRenderer::GraphicsDevice* graphicsDevice,
												  ::EffekseerRenderer::SingleFrameMemoryPool* memoryPool)
{
	auto gd = static_cast<::EffekseerRendererLLGI::GraphicsDevice*>(graphicsDevice);
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

EffekseerRenderer::SingleFrameMemoryPool* CreateSingleFrameMemoryPool(::EffekseerRenderer::GraphicsDevice* graphicsDevice)
{
	auto gd = static_cast<::EffekseerRendererLLGI::GraphicsDevice*>(graphicsDevice);
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