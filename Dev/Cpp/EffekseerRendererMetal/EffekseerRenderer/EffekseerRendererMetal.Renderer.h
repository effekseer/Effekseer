
#ifndef __EFFEKSEERRENDERER_METAL_RENDERER_H__
#define __EFFEKSEERRENDERER_METAL_RENDERER_H__

#include "../../EffekseerRendererCommon/EffekseerRenderer.Renderer.h"
#include "EffekseerRendererMetal.Base.h"
#include <functional>

#import <MetalKit/MetalKit.h>

@class MTLRenderPassDescriptor;
@protocol MTLDevice, MTLCommandBuffer, MTLRenderCommandEncoder, MTLTexture;

namespace EffekseerRendererMetal
{

::Effekseer::TextureLoaderRef CreateTextureLoader(::Effekseer::Backend::GraphicsDevice* graphicsDevice, ::Effekseer::FileInterface* fileInterface = NULL);

::Effekseer::ModelLoaderRef CreateModelLoader(::Effekseer::Backend::GraphicsDevice*graphicsDevice, ::Effekseer::FileInterface* fileInterface = NULL);

::Effekseer::MaterialLoaderRef CreateMaterialLoader(::Effekseer::Backend::GraphicsDevice*graphicsDevice, ::Effekseer::FileInterface* fileInterface = NULL);

::Effekseer::Backend::GraphicsDevice* CreateDevice();

::EffekseerRenderer::Renderer* Create(
                                      ::Effekseer::Backend::GraphicsDevice* graphicsDevice,
                                      int32_t squareMaxCount,
                                      MTLPixelFormat renderTargetFormat,
                                      MTLPixelFormat depthStencilFormat,
									  bool isReversedDepth);

/**
@brief	Create an instance
@param	device	metal device
@param	isReversedDepth is depth reversed
@param squareMaxCount	the number of maximum sprites
@return	instance
*/
::EffekseerRenderer::Renderer* Create(int32_t squareMaxCount,
                                      MTLPixelFormat renderTargetFormat,
                                      MTLPixelFormat depthStencilFormat,
									  bool isReversedDepth);

Effekseer::TextureData* CreateTextureData(::EffekseerRenderer::Renderer* renderer, id<MTLTexture> texture);

void DeleteTextureData(::EffekseerRenderer::Renderer* renderer, Effekseer::TextureData* textureData);

void FlushAndWait(::EffekseerRenderer::Renderer* renderer);

EffekseerRenderer::CommandList* CreateCommandList(::Effekseer::Backend::GraphicsDevice* graphicsDevice,
												  ::EffekseerRenderer::SingleFrameMemoryPool* memoryPool);

EffekseerRenderer::CommandList* CreateCommandList(::EffekseerRenderer::Renderer* renderer,
												  ::EffekseerRenderer::SingleFrameMemoryPool* memoryPool);

EffekseerRenderer::SingleFrameMemoryPool* CreateSingleFrameMemoryPool(::Effekseer::Backend::GraphicsDevice* graphicsDevice);

EffekseerRenderer::SingleFrameMemoryPool* CreateSingleFrameMemoryPool(::EffekseerRenderer::Renderer* renderer);

void BeginCommandList(EffekseerRenderer::CommandList* commandList, id<MTLRenderCommandEncoder> encoder);

void EndCommandList(EffekseerRenderer::CommandList* commandList);

} // namespace EffekseerRendererMetal

#endif
