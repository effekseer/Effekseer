
#ifndef __EFFEKSEERRENDERER_METAL_RENDERER_H__
#define __EFFEKSEERRENDERER_METAL_RENDERER_H__

#include "EffekseerRendererMetal.Base.Pre.h"
#include <EffekseerRendererCommon/EffekseerRenderer.Renderer.h>
#include <functional>

#import <MetalKit/MetalKit.h>

@class MTLRenderPassDescriptor;
@protocol MTLDevice
, MTLCommandBuffer, MTLRenderCommandEncoder, MTLComputeCommandEncoder, MTLTexture;

namespace EffekseerRendererMetal
{

::Effekseer::MaterialLoaderRef CreateMaterialLoader(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice, ::Effekseer::FileInterfaceRef fileInterface = NULL);

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

Effekseer::Backend::TextureRef CreateTexture(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice, id<MTLTexture> texture);

void BeginCommandList(Effekseer::RefPtr<EffekseerRenderer::CommandList> commandList);

void EndCommandList(Effekseer::RefPtr<EffekseerRenderer::CommandList> commandList);

void BeginRenderPass(Effekseer::RefPtr<EffekseerRenderer::CommandList> commandList, id<MTLRenderCommandEncoder> renderEncoder);

void EndRenderPass(Effekseer::RefPtr<EffekseerRenderer::CommandList> commandList);

void BeginComputePass(Effekseer::RefPtr<EffekseerRenderer::CommandList> commandList, id<MTLComputeCommandEncoder> computeEncoder);

void EndComputePass(Effekseer::RefPtr<EffekseerRenderer::CommandList> commandList);

} // namespace EffekseerRendererMetal

#endif
