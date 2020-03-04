#include "EffekseerRendererVulkan.Renderer.h"
#include "EffekseerMaterialCompilerVulkan.h"
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

    static const std::vector<uint8_t> standard_distorted_vert = {
#include "Shader/Vulkan/standard_distortion.vert.spv.inl"
    };
    shader->StandardDistortedTexture_VS = { { standard_distorted_vert.data(), (int32_t)standard_distorted_vert.size() } };

    static const std::vector<uint8_t> standard_frag = {
#include "Shader/Vulkan/standard.frag.spv.inl"
    };
    shader->StandardTexture_PS = { { standard_frag.data(), (int32_t)standard_frag.size() } };

    static const std::vector<uint8_t> standard_no_texture_frag = {
#include "Shader/Vulkan/standard_no_texture.frag.spv.inl"
    };

    static const std::vector<uint8_t> standard_distortion_frag = {
#include "Shader/Vulkan/standard_distortion.frag.spv.inl"
    };
    shader->StandardDistortedTexture_PS = { { standard_distortion_frag.data(), (int32_t)standard_distortion_frag.size() } };

    static const std::vector<uint8_t> standard_distortion_no_texture_frag = {
#include "Shader/Vulkan/standard_distortion_no_texture.frag.spv.inl"
    };

    static const std::vector<uint8_t> model_ltn_vert = {
#include "Shader/Vulkan/model_ltn.vert.spv.inl"
    };
    shader->ModelShaderLightingTextureNormal_VS = { { model_ltn_vert.data(), (int32_t)model_ltn_vert.size() } };

    static const std::vector<uint8_t> model_ltn_flag = {
#include "Shader/Vulkan/model_ltn.frag.spv.inl"
    };
    shader->ModelShaderLightingTextureNormal_PS = { { model_ltn_flag.data(), (int32_t)model_ltn_flag.size() } };
}

::EffekseerRenderer::Renderer* Create(
    VkPhysicalDevice physicalDevice,
    VkDevice device,
    VkQueue transfarQueue,
    VkCommandPool transfarCommandPool, 
    int32_t swapBufferCount,
    int32_t squareMaxCount)
{
    LLGI::Graphics* graphics = new LLGI::GraphicsVulkan(
        vk::Device(device),
        vk::Queue(transfarQueue),
        vk::CommandPool(transfarCommandPool),
        vk::PhysicalDevice(physicalDevice),
        swapBufferCount,
        [](vk::CommandBuffer, vk::Fence) -> void {},
        nullptr,
        nullptr);

    ::EffekseerRendererLLGI::RendererImplemented* renderer = new ::EffekseerRendererLLGI::RendererImplemented(squareMaxCount);
    CreateFixedShaderForVulkan(&renderer->fixedShader_);
    if (!renderer->Initialize(graphics, nullptr, false)) {
        ES_SAFE_RELEASE(renderer);
        ES_SAFE_RELEASE(graphics);
        return nullptr;
    }

    renderer->platformType_ = Effekseer::CompiledMaterialPlatformType::DirectX12;
    renderer->materialCompiler_ = new Effekseer::MaterialCompilerVulkan();

    ES_SAFE_RELEASE(graphics);

	return renderer;
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