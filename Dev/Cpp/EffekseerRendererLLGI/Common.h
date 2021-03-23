
#ifndef __EFFEKSEERRENDERER_LLGI_COMMON_H__
#define __EFFEKSEERRENDERER_LLGI_COMMON_H__

#include "../EffekseerRendererCommon/EffekseerRenderer.Renderer.h"
#include <Effekseer.h>

namespace EffekseerRenderer
{

void FlushAndWait(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice);

Effekseer::RefPtr<EffekseerRenderer::CommandList> CreateCommandList(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice,
																	Effekseer::RefPtr<::EffekseerRenderer::SingleFrameMemoryPool> memoryPool);

Effekseer::RefPtr<EffekseerRenderer::SingleFrameMemoryPool> CreateSingleFrameMemoryPool(::Effekseer::Backend::GraphicsDeviceRef renderer);

} // namespace EffekseerRenderer

#endif