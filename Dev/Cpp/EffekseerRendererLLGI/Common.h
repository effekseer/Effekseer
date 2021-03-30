
#ifndef __EFFEKSEERRENDERER_LLGI_COMMON_H__
#define __EFFEKSEERRENDERER_LLGI_COMMON_H__

#include "../EffekseerRendererCommon/EffekseerRenderer.Renderer.h"
#include <Effekseer.h>

namespace EffekseerRenderer
{

struct RenderPassPipelineStateKey
{
	std::array<Effekseer::Backend::TextureFormatType, 8> RenderTargetFormats;
	int32_t RenderTargetCount = 0;
	Effekseer::Backend::TextureFormatType DepthFormat = Effekseer::Backend::TextureFormatType::Unknown;
};

void ChangeRenderPassPipelineState(EffekseerRenderer::RendererRef renderer, RenderPassPipelineStateKey key);

void FlushAndWait(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice);

Effekseer::RefPtr<EffekseerRenderer::CommandList> CreateCommandList(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice,
																	Effekseer::RefPtr<::EffekseerRenderer::SingleFrameMemoryPool> memoryPool);

Effekseer::RefPtr<EffekseerRenderer::SingleFrameMemoryPool> CreateSingleFrameMemoryPool(::Effekseer::Backend::GraphicsDeviceRef renderer);

} // namespace EffekseerRenderer

#endif