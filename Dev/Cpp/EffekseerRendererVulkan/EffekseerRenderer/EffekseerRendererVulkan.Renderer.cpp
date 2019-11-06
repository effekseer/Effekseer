#include "EffekseerRendererVulkan.Renderer.h"
#include "../../3rdParty/LLGI/src/Vulkan/LLGI.CommandListVulkan.h"
#include "../../3rdParty/LLGI/src/Vulkan/LLGI.GraphicsVulkan.h"
#include "../EffekseerRendererLLGI/EffekseerRendererLLGI.RendererImplemented.h"

namespace EffekseerRendererVulkan
{

::EffekseerRenderer::Renderer* Create(int32_t squareMaxCount)
{
    throw "Not implemented.";
	return nullptr;
}

Effekseer::TextureData* CreateTextureData(::EffekseerRenderer::Renderer* renderer, VkImage texture)
{
    throw "Not implemented.";
    return nullptr;
}

void DeleteTextureData(::EffekseerRenderer::Renderer* renderer, Effekseer::TextureData* textureData)
{
    throw "Not implemented.";
}

void FlushAndWait(::EffekseerRenderer::Renderer* renderer)
{
    throw "Not implemented.";
}

EffekseerRenderer::CommandList* CreateCommandList(::EffekseerRenderer::Renderer* renderer,
												  ::EffekseerRenderer::SingleFrameMemoryPool* memoryPool)
{
    throw "Not implemented.";
    return nullptr;
}

EffekseerRenderer::SingleFrameMemoryPool* CreateSingleFrameMemoryPool(::EffekseerRenderer::Renderer* renderer)
{
    throw "Not implemented.";
    return nullptr;
}

void BeginCommandList(EffekseerRenderer::CommandList* commandList, VkCommandBuffer nativeCommandList)
{
    throw "Not implemented.";
}

void EndCommandList(EffekseerRenderer::CommandList* commandList)
{
    throw "Not implemented.";
}

} // namespace EffekseerRendererVulkan