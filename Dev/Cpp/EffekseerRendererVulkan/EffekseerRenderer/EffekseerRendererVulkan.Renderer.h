
#ifndef __EFFEKSEERRENDERER_VULKAN_RENDERER_H__
#define __EFFEKSEERRENDERER_VULKAN_RENDERER_H__

#include "../../EffekseerRendererCommon/EffekseerRenderer.Renderer.h"
#include "EffekseerRendererVulkan.Base.h"
#include <functional>

namespace EffekseerRendererVulkan
{

struct VulkanImageInfo
{
	VkImage image;
	VkImageAspectFlags aspect;
	VkFormat format;
};

struct RenderPassInformation
{
	bool DoesPresentToScreen = false;
	std::array<VkFormat, 8> RenderTextureFormats;
	int32_t RenderTextureCount = 1;
	VkFormat DepthFormat = VK_FORMAT_UNDEFINED;
};

::Effekseer::Backend::GraphicsDevice* CreateGraphicsDevice(
	VkPhysicalDevice physicalDevice, VkDevice device, VkQueue transfarQueue, VkCommandPool transfarCommandPool, int32_t swapBufferCount);

::EffekseerRenderer::Renderer*
Create(::Effekseer::Backend::GraphicsDevice* graphicsDevice, RenderPassInformation renderPassInformation, int32_t squareMaxCount);

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

Effekseer::TextureData* CreateTextureData(::EffekseerRenderer::Renderer* renderer, const VulkanImageInfo& info);

Effekseer::TextureData* CreateTextureData(::Effekseer::Backend::GraphicsDevice* graphicsDevice, const VulkanImageInfo& info);

void DeleteTextureData(::EffekseerRenderer::Renderer* renderer, Effekseer::TextureData* textureData);

void DeleteTextureData(::Effekseer::Backend::GraphicsDevice* graphicsDevice, Effekseer::TextureData* textureData);

void FlushAndWait(::EffekseerRenderer::Renderer* renderer);

void FlushAndWait(::Effekseer::Backend::GraphicsDevice* graphicsDevice);

EffekseerRenderer::CommandList* CreateCommandList(::EffekseerRenderer::Renderer* renderer,
												  ::EffekseerRenderer::SingleFrameMemoryPool* memoryPool);

EffekseerRenderer::CommandList* CreateCommandList(::Effekseer::Backend::GraphicsDevice* graphicsDevice,
												  ::EffekseerRenderer::SingleFrameMemoryPool* memoryPool);

EffekseerRenderer::SingleFrameMemoryPool* CreateSingleFrameMemoryPool(::EffekseerRenderer::Renderer* renderer);

EffekseerRenderer::SingleFrameMemoryPool* CreateSingleFrameMemoryPool(::Effekseer::Backend::GraphicsDevice* graphicsDevice);

void BeginCommandList(EffekseerRenderer::CommandList* commandList, VkCommandBuffer nativeCommandList);

void EndCommandList(EffekseerRenderer::CommandList* commandList);

} // namespace EffekseerRendererVulkan

#endif