
#ifndef __EFFEKSEERRENDERER_DX12_RENDERER_H__
#define __EFFEKSEERRENDERER_DX12_RENDERER_H__

#include "../../EffekseerRendererCommon/EffekseerRenderer.Renderer.h"
#include "EffekseerRendererDX12.Base.h"
#include <functional>

namespace EffekseerRendererDX12
{

::Effekseer::Backend::GraphicsDeviceRef CreateGraphicsDevice(ID3D12Device* device, ID3D12CommandQueue* commandQueue, int32_t swapBufferCount);

/**
	@brief	Create an instance
	@param	graphicsDevice	Effekseer graphics device
	@param	renderTargetFormats	Rendered screen formats
	@param	renderTargetCount	The number of render target
	@param	depthFormat	Rendered screen depth format. If depth doesn't exists, it must be DXGI_FORMAT_UNKNOWN
	@param	isReversedDepth	Whether depth is reversed.
	@param	squareMaxCount	The number of maximum sprites
	@return	instance
*/
::EffekseerRenderer::RendererRef Create(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice,
									  DXGI_FORMAT* renderTargetFormats,
									  int32_t renderTargetCount,
									  DXGI_FORMAT depthFormat,
									  bool isReversedDepth,
									  int32_t squareMaxCount);

/**
	@brief	Create an instance
	@param	device	directX12 device
	@param	commandQueue directX12 commandQueue
	@param	swapBufferCount	The number of swapBufferCount
	@param	renderTargetFormats	Rendered screen formats
	@param	renderTargetCount	The number of render target
	@param	depthFormat	Rendered screen depth format. If depth doesn't exists, it must be DXGI_FORMAT_UNKNOWN
	@param	isReversedDepth	Whether depth is reversed.
	@param	squareMaxCount	The number of maximum sprites
	@return	instance
*/
::EffekseerRenderer::RendererRef Create(ID3D12Device* device,
									  ID3D12CommandQueue* commandQueue,
									  int32_t swapBufferCount,
									  DXGI_FORMAT* renderTargetFormats,
									  int32_t renderTargetCount,
									  DXGI_FORMAT depthFormat,
									  bool isReversedDepth,
									  int32_t squareMaxCount);

Effekseer::Backend::TextureRef CreateTexture(::EffekseerRenderer::RendererRef renderer, ID3D12Resource* texture);

Effekseer::Backend::TextureRef CreateTexture(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice, ID3D12Resource* texture);

void FlushAndWait(::EffekseerRenderer::RendererRef renderer);

void FlushAndWait(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice);

EffekseerRenderer::CommandList* CreateCommandList(::EffekseerRenderer::RendererRef renderer,
												  ::EffekseerRenderer::SingleFrameMemoryPool* memoryPool);

EffekseerRenderer::CommandList* CreateCommandList(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice,
												  ::EffekseerRenderer::SingleFrameMemoryPool* memoryPool);

EffekseerRenderer::SingleFrameMemoryPool* CreateSingleFrameMemoryPool(::EffekseerRenderer::RendererRef renderer);

EffekseerRenderer::SingleFrameMemoryPool* CreateSingleFrameMemoryPool(::Effekseer::Backend::GraphicsDeviceRef renderer);

void BeginCommandList(EffekseerRenderer::CommandList* commandList, ID3D12GraphicsCommandList* dx12CommandList);

void EndCommandList(EffekseerRenderer::CommandList* commandList);

} // namespace EffekseerRendererDX12

#endif