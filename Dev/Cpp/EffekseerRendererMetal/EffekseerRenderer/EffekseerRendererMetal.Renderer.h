
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

::Effekseer::TextureLoader* CreateTextureLoader(::EffekseerRenderer::GraphicsDevice* graphicsDevice, ::Effekseer::FileInterface* fileInterface = NULL);

::Effekseer::ModelLoader* CreateModelLoader(::EffekseerRenderer::GraphicsDevice*graphicsDevice, ::Effekseer::FileInterface* fileInterface = NULL);

::Effekseer::MaterialLoader* CreateMaterialLoader(::EffekseerRenderer::GraphicsDevice*graphicsDevice, ::Effekseer::FileInterface* fileInterface = NULL);

::EffekseerRenderer::GraphicsDevice* CreateDevice();

::EffekseerRenderer::Renderer* Create(
                                      ::EffekseerRenderer::GraphicsDevice* graphicsDevice,
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

EffekseerRenderer::CommandList* CreateCommandList(::EffekseerRenderer::GraphicsDevice* graphicsDevice,
												  ::EffekseerRenderer::SingleFrameMemoryPool* memoryPool);

EffekseerRenderer::CommandList* CreateCommandList(::EffekseerRenderer::Renderer* renderer,
												  ::EffekseerRenderer::SingleFrameMemoryPool* memoryPool);

EffekseerRenderer::SingleFrameMemoryPool* CreateSingleFrameMemoryPool(::EffekseerRenderer::GraphicsDevice* graphicsDevice);

EffekseerRenderer::SingleFrameMemoryPool* CreateSingleFrameMemoryPool(::EffekseerRenderer::Renderer* renderer);

void BeginCommandList(EffekseerRenderer::CommandList* commandList, id<MTLRenderCommandEncoder> encoder);

void EndCommandList(EffekseerRenderer::CommandList* commandList);

} // namespace EffekseerRendererMetal

#endif
