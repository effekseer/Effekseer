#ifndef __EFFEKSEERRENDERER_WEBGPU_RENDERER_H__
#define __EFFEKSEERRENDERER_WEBGPU_RENDERER_H__

#include "EffekseerRendererWebGPU.Base.Pre.h"
#include <EffekseerRendererCommon/EffekseerRenderer.Renderer.h>
#include <array>

namespace EffekseerRendererWebGPU
{

struct RenderPassInformation
{
	bool DoesPresentToScreen = false;
	std::array<wgpu::TextureFormat, 8> RenderTextureFormats;
	int32_t RenderTextureCount = 1;
	wgpu::TextureFormat DepthFormat = wgpu::TextureFormat::Undefined;
};

::Effekseer::Backend::GraphicsDeviceRef CreateGraphicsDevice(wgpu::Device device);

::Effekseer::Backend::GraphicsDeviceRef CreateGraphicsDevice(wgpu::Device device, wgpu::Instance instance);

::EffekseerRenderer::RendererRef
Create(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice, RenderPassInformation renderPassInformation, int32_t squareMaxCount);

::EffekseerRenderer::RendererRef Create(wgpu::Device device, RenderPassInformation renderPassInformation, int32_t squareMaxCount);

::EffekseerRenderer::RendererRef
Create(wgpu::Device device, wgpu::Instance instance, RenderPassInformation renderPassInformation, int32_t squareMaxCount);

void BeginCommandList(Effekseer::RefPtr<EffekseerRenderer::CommandList> commandList);

void EndCommandList(Effekseer::RefPtr<EffekseerRenderer::CommandList> commandList);

void ExecuteCommandList(Effekseer::RefPtr<EffekseerRenderer::CommandList> commandList);

} // namespace EffekseerRendererWebGPU

#endif
