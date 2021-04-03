
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

Effekseer::Backend::TextureRef CreateTexture(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice, ID3D12Resource* texture);

/**
		@brief	\~English	Properties in a texture
				\~Japanese	テクスチャ内のプロパティ

		@note	\~English	You need not to release pointers
				\~Japanese	ポインタの解放する必要はない
*/
struct TextureProperty
{
	ID3D12Resource* TexturePtr = nullptr;
};

TextureProperty GetTextureProperty(::Effekseer::Backend::TextureRef texture);

/**
		@brief	\~English	Properties in a commandlist
				\~Japanese	コマンドリスト内のプロパティ

		@note	\~English	You need not to release pointers
				\~Japanese	ポインタの解放する必要はない
*/
struct CommandListProperty
{
	ID3D12CommandList* CommandListPtr = nullptr;
};

CommandListProperty GetCommandListProperty(Effekseer::RefPtr<EffekseerRenderer::CommandList> commandList);

/**
	@brief	\~English	Start to record commands
	\~Japanese	コマンドの記録を開始する。
	@note
	\~English	dx12CommandList can be null. In this case, it need to call ExecuteCommandList
	\~Japanese	dx12CommandList はnullにできる。その場合、ExecuteCommandListを呼ぶ必要がある。
*/
void BeginCommandList(Effekseer::RefPtr<EffekseerRenderer::CommandList> commandList, ID3D12GraphicsCommandList* dx12CommandList);

void EndCommandList(Effekseer::RefPtr<EffekseerRenderer::CommandList> commandList);

void ExecuteCommandList(Effekseer::RefPtr<EffekseerRenderer::CommandList> commandList);

} // namespace EffekseerRendererDX12

#endif