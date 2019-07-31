
#ifndef __EFFEKSEERRENDERER_DX12_RENDERER_H__
#define __EFFEKSEERRENDERER_DX12_RENDERER_H__

#include "../../EffekseerRendererCommon/EffekseerRenderer.Renderer.h"
#include "EffekseerRendererDX12.Base.h"
#include <functional>

namespace EffekseerRendererDX12
{
/**
@brief	Create an instance
@param	device			directX12 device
@param int32_t	the number of maximum sprites
@return	instance
*/
::EffekseerRenderer::Renderer* Create(ID3D12Device* device,
									  int32_t swapBufferCount,
									  ID3D12CommandQueue* commandQueue,
									  std::function<void()> flushAndWaitQueueFunc,
									  bool isReversedDepth,
									  int32_t squareMaxCount);

Effekseer::TextureData* CreateTextureData(::EffekseerRenderer::Renderer* renderer, ID3D12Resource* texture);

void DeleteTextureData(::EffekseerRenderer::Renderer* renderer, Effekseer::TextureData* textureData);

void FlushAndWait(::EffekseerRenderer::Renderer* renderer);

EffekseerRenderer::CommandList* CreateCommandList(::EffekseerRenderer::Renderer* renderer, ::EffekseerRenderer::SingleFrameMemoryPool* memoryPool);

EffekseerRenderer::SingleFrameMemoryPool* CreateSingleFrameMemoryPool(::EffekseerRenderer::Renderer* renderer);

void BeginCommandList(EffekseerRenderer::CommandList* commandList);

void EndCommandList(EffekseerRenderer::CommandList* commandList);

void ExecuteCommandList(EffekseerRenderer::CommandList* commandList);

} // namespace EffekseerRendererDX12

#endif