#include "EffekseerRendererVulkan.Renderer.h"
#include "../../3rdParty/LLGI/src/Vulkan/LLGI.CommandListVulkan.h"
#include "../../3rdParty/LLGI/src/Vulkan/LLGI.GraphicsVulkan.h"
#include "../../3rdParty/LLGI/src/Vulkan/LLGI.CommandListVulkan.h"
#include "../EffekseerRendererLLGI/EffekseerRendererLLGI.RendererImplemented.h"
#include "EffekseerMaterialCompilerVulkan.h"


namespace EffekseerRendererVulkan
{

static void CreateFixedShaderForVulkan(EffekseerRendererLLGI::FixedShader* shader)
{
	assert(shader);
	if (!shader)
		return;

	static const std::vector<uint8_t> standard_vert = {
#include "Shader/Vulkan/standard.vert.spv.inl"
	};
	shader->StandardTexture_VS = {{standard_vert.data(), (int32_t)standard_vert.size()}};

	static const std::vector<uint8_t> standard_distorted_vert = {
#include "Shader/Vulkan/standard_distortion.vert.spv.inl"
	};
	shader->StandardDistortedTexture_VS = {{standard_distorted_vert.data(), (int32_t)standard_distorted_vert.size()}};

	static const std::vector<uint8_t> standard_frag = {
#include "Shader/Vulkan/standard.frag.spv.inl"
	};
	shader->StandardTexture_PS = {{standard_frag.data(), (int32_t)standard_frag.size()}};

	static const std::vector<uint8_t> standard_distortion_frag = {
#include "Shader/Vulkan/standard_distortion.frag.spv.inl"
	};
	shader->StandardDistortedTexture_PS = {{standard_distortion_frag.data(), (int32_t)standard_distortion_frag.size()}};

	static const std::vector<uint8_t> model_ltn_vert = {
#include "Shader/Vulkan/model_ltn.vert.spv.inl"
	};
	shader->ModelShaderLightingTextureNormal_VS = {{model_ltn_vert.data(), (int32_t)model_ltn_vert.size()}};

	static const std::vector<uint8_t> model_ltn_flag = {
#include "Shader/Vulkan/model_ltn.frag.spv.inl"
	};
	shader->ModelShaderLightingTextureNormal_PS = {{model_ltn_flag.data(), (int32_t)model_ltn_flag.size()}};
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

	key.HasDepth = renderPassInformation.HasDepth;

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

Effekseer::TextureData* CreateTextureData(::EffekseerRenderer::Renderer* renderer, VkImage texture)
{
	auto r = static_cast<::EffekseerRendererLLGI::RendererImplemented*>(renderer);
	return CreateTextureData(r->GetGraphicsDevice(), texture);
}

Effekseer::TextureData* CreateTextureData(::EffekseerRenderer::GraphicsDevice* graphicsDevice, VkImage texture)
{
	auto g = static_cast<::EffekseerRendererLLGI::GraphicsDevice*>(graphicsDevice);
	auto texture_ = g->GetGraphics()->CreateTexture((uint64_t)texture);

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