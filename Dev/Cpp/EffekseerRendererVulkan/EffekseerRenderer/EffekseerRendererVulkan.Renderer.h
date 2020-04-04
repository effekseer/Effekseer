
#ifndef __EFFEKSEERRENDERER_VULKAN_RENDERER_H__
#define __EFFEKSEERRENDERER_VULKAN_RENDERER_H__

#include "../../EffekseerRendererCommon/EffekseerRenderer.Renderer.h"
#include "EffekseerRendererVulkan.Base.h"
#include <functional>

namespace EffekseerRendererVulkan
{


struct RenderPassInformation
{
	bool DoesPresentToScreen = false;
	std::array<VkFormat, 8> RenderTextureFormats;
	int32_t RenderTextureCount = 1;
	bool HasDepth = false;
};

::EffekseerRenderer::GraphicsDevice* CreateDevice(
	VkPhysicalDevice physicalDevice, VkDevice device, VkQueue transfarQueue, VkCommandPool transfarCommandPool, int32_t swapBufferCount);

::EffekseerRenderer::Renderer*
Create(::EffekseerRenderer::GraphicsDevice* graphicsDevice, RenderPassInformation renderPassInformation, int32_t squareMaxCount);

/**
@brief	Create an instance
@param transfarQueue   Used in short-time command buffer for immediate data transfer.
@param transfarCommandPool   Used in short-time command buffer for immediate data transfer.
@param squareMaxCount	the number of maximum sprites
@return	instance
*/
::EffekseerRenderer::Renderer* Create(VkPhysicalDevice physicalDevice,
									  VkDevice device,
									  VkQueue transfarQueue,
									  VkCommandPool transfarCommandPool,
									  int32_t swapBufferCount,
									  RenderPassInformation renderPassInformation,
									  int32_t squareMaxCount);

Effekseer::TextureData* CreateTextureData(::EffekseerRenderer::Renderer* renderer, VkImage texture);

Effekseer::TextureData* CreateTextureData(::EffekseerRenderer::GraphicsDevice* graphicsDevice, VkImage texture);

void DeleteTextureData(::EffekseerRenderer::Renderer* renderer, Effekseer::TextureData* textureData);

void DeleteTextureData(::EffekseerRenderer::GraphicsDevice* graphicsDevice, Effekseer::TextureData* textureData);

void FlushAndWait(::EffekseerRenderer::Renderer* renderer);

void FlushAndWait(::EffekseerRenderer::GraphicsDevice* graphicsDevice);

EffekseerRenderer::CommandList* CreateCommandList(::EffekseerRenderer::Renderer* renderer,
												  ::EffekseerRenderer::SingleFrameMemoryPool* memoryPool);

EffekseerRenderer::CommandList* CreateCommandList(::EffekseerRenderer::GraphicsDevice* graphicsDevice,
												  ::EffekseerRenderer::SingleFrameMemoryPool* memoryPool);

EffekseerRenderer::SingleFrameMemoryPool* CreateSingleFrameMemoryPool(::EffekseerRenderer::Renderer* renderer);

EffekseerRenderer::SingleFrameMemoryPool* CreateSingleFrameMemoryPool(::EffekseerRenderer::GraphicsDevice* graphicsDevice);

void BeginCommandList(EffekseerRenderer::CommandList* commandList, VkCommandBuffer nativeCommandList);

void EndCommandList(EffekseerRenderer::CommandList* commandList);

} // namespace EffekseerRendererVulkan

#endif