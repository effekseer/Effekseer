
#ifndef __EFFEKSEERRENDERER_VULKAN_RENDERER_H__
#define __EFFEKSEERRENDERER_VULKAN_RENDERER_H__

#include "../../EffekseerRendererCommon/EffekseerRenderer.Renderer.h"
#include "EffekseerRendererVulkan.Base.h"
#include <functional>

namespace EffekseerRendererVulkan
{
/**
@brief	Create an instance
@params transfarQueue   Used in short-time command buffer for immediate data transfer.
@params transfarCommandPool   Used in short-time command buffer for immediate data transfer.
@param int32_t	the number of maximum sprites
@return	instance
@note
    The current EffekseerRendererVulkan is intended for incorporation into an externally implemented Vulkan rendering engine.

    It does not work standalone.

    Therefore, unlike EffekseerRendererDX12, the following objects need to be created externally.

    - LLGI::RenderPass
        - It cannot be created internally, because it depends on the VkImage that represents the render target.
    - LLGI::RenderPassPipelineState
        - It cannot be created internally, because it depends on LLGI::RenderPass.

    LLGI::RenderPassPipelineState must be set using EffekseerRendererLLGI::RendererImplemented::SetRenderPassPipelineState().

    You can draw by calling Manager::Draw() with LLGI::RenderPass started.

    See Example/Vulkan for an implementation example.
*/
::EffekseerRenderer::Renderer* Create(
    VkPhysicalDevice physicalDevice,
    VkDevice device,
    VkQueue transfarQueue,
    VkCommandPool transfarCommandPool,
    int32_t swapBufferCount,
    int32_t squareMaxCount);

Effekseer::TextureData* CreateTextureData(::EffekseerRenderer::Renderer* renderer, VkImage texture);

void DeleteTextureData(::EffekseerRenderer::Renderer* renderer, Effekseer::TextureData* textureData);

void FlushAndWait(::EffekseerRenderer::Renderer* renderer);

EffekseerRenderer::CommandList* CreateCommandList(::EffekseerRenderer::Renderer* renderer,
												  ::EffekseerRenderer::SingleFrameMemoryPool* memoryPool);

EffekseerRenderer::SingleFrameMemoryPool* CreateSingleFrameMemoryPool(::EffekseerRenderer::Renderer* renderer);

void BeginCommandList(EffekseerRenderer::CommandList* commandList, VkCommandBuffer nativeCommandList);

void EndCommandList(EffekseerRenderer::CommandList* commandList);

} // namespace EffekseerRendererVulkan

#endif