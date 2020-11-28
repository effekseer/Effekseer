
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

::Effekseer::Backend::GraphicsDeviceRef CreateGraphicsDevice(
	VkPhysicalDevice physicalDevice, VkDevice device, VkQueue transfarQueue, VkCommandPool transfarCommandPool, int32_t swapBufferCount);

::EffekseerRenderer::RendererRef
Create(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice, RenderPassInformation renderPassInformation, int32_t squareMaxCount);

/**
@brief	Create an instance
@param transfarQueue   Used in short-time command buffer for immediate data transfer.
@param transfarCommandPool   Used in short-time command buffer for immediate data transfer.
@param squareMaxCount	the number of maximum sprites
@return	instance
*/
::EffekseerRenderer::RendererRef Create(VkPhysicalDevice physicalDevice,
									  VkDevice device,
									  VkQueue transfarQueue,
									  VkCommandPool transfarCommandPool,
									  int32_t swapBufferCount,
									  RenderPassInformation renderPassInformation,
									  int32_t squareMaxCount);

Effekseer::TextureData* CreateTextureData(::EffekseerRenderer::Renderer* renderer, const VulkanImageInfo& info);

Effekseer::TextureData* CreateTextureData(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice, const VulkanImageInfo& info);

void DeleteTextureData(Effekseer::TextureData* textureData);

void FlushAndWait(::EffekseerRenderer::Renderer* renderer);

void FlushAndWait(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice);

EffekseerRenderer::CommandList* CreateCommandList(::EffekseerRenderer::RendererRef renderer,
												  ::EffekseerRenderer::SingleFrameMemoryPool* memoryPool);

EffekseerRenderer::CommandList* CreateCommandList(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice,
												  ::EffekseerRenderer::SingleFrameMemoryPool* memoryPool);

EffekseerRenderer::SingleFrameMemoryPool* CreateSingleFrameMemoryPool(::EffekseerRenderer::RendererRef renderer);

EffekseerRenderer::SingleFrameMemoryPool* CreateSingleFrameMemoryPool(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice);

void BeginCommandList(EffekseerRenderer::CommandList* commandList, VkCommandBuffer nativeCommandList);

void EndCommandList(EffekseerRenderer::CommandList* commandList);

} // namespace EffekseerRendererVulkan

#endif