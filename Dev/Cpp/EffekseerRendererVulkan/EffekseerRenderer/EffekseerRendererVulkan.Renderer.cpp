#include "EffekseerRendererVulkan.Renderer.h"
#include "../../3rdParty/LLGI/src/Vulkan/LLGI.CommandListVulkan.h"
#include "../../3rdParty/LLGI/src/Vulkan/LLGI.GraphicsVulkan.h"
#include "../EffekseerRendererLLGI/EffekseerRendererLLGI.RendererImplemented.h"

namespace EffekseerRendererVulkan
{


static void CreateFixedShaderForVulkan(EffekseerRendererLLGI::FixedShader* shader)
{
    assert(shader);
    if (!shader) return;

    static const std::vector<uint8_t> standard_vert = {
#include "Shader/Vulkan/standard.vert.spv.inl"
    };
    shader->StandardTexture_VS = { { standard_vert.data(), (int32_t)standard_vert.size() } };
    //shader->Standard_VS = { { standard_vert.data(), standard_vert.size() } };

    static const std::vector<uint8_t> standard_distorted_vert = {
#include "Shader/Vulkan/standard_distortion.vert.spv.inl"
    };
    shader->StandardDistortedTexture_VS = { { standard_distorted_vert.data(), (int32_t)standard_distorted_vert.size() } };
    //shader->StandardDistorted_VS = { { standard_distorted_vert.data(), standard_distorted_vert.size() } };

    static const std::vector<uint8_t> standard_frag = {
#include "Shader/Vulkan/standard.frag.spv.inl"
    };
    shader->StandardTexture_PS = { { standard_frag.data(), (int32_t)standard_frag.size() } };

    static const std::vector<uint8_t> standard_no_texture_frag = {
#include "Shader/Vulkan/standard_no_texture.frag.spv.inl"
    };
    //shader->Standard_PS = { { standard_no_texture_frag.data(), standard_no_texture_frag.size() } };

    static const std::vector<uint8_t> standard_distortion_frag = {
#include "Shader/Vulkan/standard_distortion.frag.spv.inl"
    };
    shader->StandardDistortedTexture_PS = { { standard_distortion_frag.data(), (int32_t)standard_distortion_frag.size() } };

    static const std::vector<uint8_t> standard_distortion_no_texture_frag = {
#include "Shader/Vulkan/standard_distortion_no_texture.frag.spv.inl"
    };
    //shader->StandardDistorted_PS = { { standard_distortion_no_texture_frag.data(), standard_distortion_no_texture_frag.size() } };
}

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