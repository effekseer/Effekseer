
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

::Effekseer::TextureLoaderRef CreateTextureLoader(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice, ::Effekseer::FileInterface* fileInterface = NULL);

::Effekseer::ModelLoaderRef CreateModelLoader(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice, ::Effekseer::FileInterface* fileInterface = NULL);

::Effekseer::MaterialLoaderRef CreateMaterialLoader(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice, ::Effekseer::FileInterface* fileInterface = NULL);

::Effekseer::Backend::GraphicsDeviceRef CreateDevice();

::EffekseerRenderer::RendererRef Create(
                                      ::Effekseer::Backend::GraphicsDeviceRef graphicsDevice,
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
::EffekseerRenderer::RendererRef Create(int32_t squareMaxCount,
                                      MTLPixelFormat renderTargetFormat,
                                      MTLPixelFormat depthStencilFormat,
									  bool isReversedDepth);

Effekseer::Backend::TextureRef CreateTexture(::EffekseerRenderer::RendererRef renderer, id<MTLTexture> texture);

Effekseer::Backend::TextureRef CreateTexture(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice, id<MTLTexture> texture);

void FlushAndWait(::EffekseerRenderer::RendererRef renderer);

EffekseerRenderer::CommandList* CreateCommandList(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice,
												  ::EffekseerRenderer::SingleFrameMemoryPool* memoryPool);

EffekseerRenderer::CommandList* CreateCommandList(::EffekseerRenderer::RendererRef renderer,
												  ::EffekseerRenderer::SingleFrameMemoryPool* memoryPool);

EffekseerRenderer::SingleFrameMemoryPool* CreateSingleFrameMemoryPool(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice);

EffekseerRenderer::SingleFrameMemoryPool* CreateSingleFrameMemoryPool(::EffekseerRenderer::RendererRef Refrenderer);

void BeginCommandList(EffekseerRenderer::CommandList* commandList, id<MTLRenderCommandEncoder> encoder);

void EndCommandList(EffekseerRenderer::CommandList* commandList);

} // namespace EffekseerRendererMetal

#endif
