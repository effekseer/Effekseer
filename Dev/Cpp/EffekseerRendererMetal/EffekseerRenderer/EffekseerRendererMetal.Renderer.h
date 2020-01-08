
#ifndef __EFFEKSEERRENDERER_METAL_RENDERER_H__
#define __EFFEKSEERRENDERER_METAL_RENDERER_H__

#include "../../EffekseerRendererCommon/EffekseerRenderer.Renderer.h"
#include "EffekseerRendererMetal.Base.h"
#include <functional>

@class MTLRenderPassDescriptor;
@protocol MTLDevice, MTLCommandBuffer, MTLRenderCommandEncoder, MTLTexture;

namespace EffekseerRendererMetal
{
/**
@brief	Create an instance
@param	device	metal device
@param	isReversedDepth is depth reversed
@param squareMaxCount	the number of maximum sprites
@return	instance
*/
::EffekseerRenderer::Renderer* Create(id<MTLDevice> device,
									  bool isReversedDepth,
									  int32_t squareMaxCount);

Effekseer::TextureData* CreateTextureData(::EffekseerRenderer::Renderer* renderer, id<MTLTexture> texture);

void DeleteTextureData(::EffekseerRenderer::Renderer* renderer, Effekseer::TextureData* textureData);

void FlushAndWait(::EffekseerRenderer::Renderer* renderer);

EffekseerRenderer::CommandList* CreateCommandList(::EffekseerRenderer::Renderer* renderer,
												  ::EffekseerRenderer::SingleFrameMemoryPool* memoryPool);

EffekseerRenderer::SingleFrameMemoryPool* CreateSingleFrameMemoryPool(::EffekseerRenderer::Renderer* renderer);

void BeginCommandList(EffekseerRenderer::CommandList* commandList, id<MTLCommandBuffer> commandBuffer);

void EndCommandList(EffekseerRenderer::CommandList* commandList);

} // namespace EffekseerRendererMetal

#endif