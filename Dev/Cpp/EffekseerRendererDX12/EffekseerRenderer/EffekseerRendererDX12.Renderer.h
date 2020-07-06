
#ifndef __EFFEKSEERRENDERER_DX12_RENDERER_H__
#define __EFFEKSEERRENDERER_DX12_RENDERER_H__

#include "../../EffekseerRendererCommon/EffekseerRenderer.Renderer.h"
#include "EffekseerRendererDX12.Base.h"
#include <functional>

namespace EffekseerRendererDX12
{

::EffekseerRenderer::GraphicsDevice* CreateDevice(ID3D12Device* device, ID3D12CommandQueue* commandQueue, int32_t swapBufferCount);

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
::EffekseerRenderer::Renderer* Create(::EffekseerRenderer::GraphicsDevice* graphicsDevice,
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
::EffekseerRenderer::Renderer* Create(ID3D12Device* device,
									  ID3D12CommandQueue* commandQueue,
									  int32_t swapBufferCount,
									  DXGI_FORMAT* renderTargetFormats,
									  int32_t renderTargetCount,
									  DXGI_FORMAT depthFormat,
									  bool isReversedDepth,
									  int32_t squareMaxCount);

Effekseer::TextureData* CreateTextureData(::EffekseerRenderer::Renderer* renderer, ID3D12Resource* texture);

Effekseer::TextureData* CreateTextureData(::EffekseerRenderer::GraphicsDevice* graphicsDevice, ID3D12Resource* texture);

void DeleteTextureData(::EffekseerRenderer::Renderer* renderer, Effekseer::TextureData* textureData);

void DeleteTextureData(::EffekseerRenderer::GraphicsDevice* graphicsDevice, Effekseer::TextureData* textureData);

void FlushAndWait(::EffekseerRenderer::Renderer* renderer);

void FlushAndWait(::EffekseerRenderer::GraphicsDevice* graphicsDevice);

EffekseerRenderer::CommandList* CreateCommandList(::EffekseerRenderer::Renderer* renderer,
												  ::EffekseerRenderer::SingleFrameMemoryPool* memoryPool);

EffekseerRenderer::CommandList* CreateCommandList(::EffekseerRenderer::GraphicsDevice* graphicsDevice,
												  ::EffekseerRenderer::SingleFrameMemoryPool* memoryPool);

EffekseerRenderer::SingleFrameMemoryPool* CreateSingleFrameMemoryPool(::EffekseerRenderer::Renderer* renderer);

EffekseerRenderer::SingleFrameMemoryPool* CreateSingleFrameMemoryPool(::EffekseerRenderer::GraphicsDevice* renderer);

void BeginCommandList(EffekseerRenderer::CommandList* commandList, ID3D12GraphicsCommandList* dx12CommandList);

void EndCommandList(EffekseerRenderer::CommandList* commandList);

} // namespace EffekseerRendererDX12

#endif