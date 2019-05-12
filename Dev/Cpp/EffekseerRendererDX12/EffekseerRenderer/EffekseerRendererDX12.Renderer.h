
#ifndef __EFFEKSEERRENDERER_DX12_RENDERER_H__
#define __EFFEKSEERRENDERER_DX12_RENDERER_H__

#include "../../EffekseerRendererCommon/EffekseerRenderer.Renderer.h"
#include "EffekseerRendererDX12.Base.h"
#include <functional>

namespace EffekseerRendererDX12
{

/**
	@brief	A class for render effects
*/
class Renderer : public ::EffekseerRenderer::Renderer
{
protected:
	Renderer() {}
	virtual ~Renderer() {}

public:
	/**
	@brief	Create an instance
	@param	device			directX12 device
	@param int32_t	the number of maximum sprites
	@return	instance
	*/
	static Renderer* Create(ID3D12Device* device,
							int32_t swapBufferCount,
							ID3D12CommandQueue* commandQueue,
							std::function<void()> flushAndWaitQueueFunc,
							bool isReversedDepth,
							int32_t squareMaxCount);

	/**
	@brief	\~English	notify that new frame is started
	\~Japanese	新規フレームが始ったことを通知する
	*/
	virtual void NewFrame() = 0;

	virtual Effekseer::TextureData* CreateTextureData(ID3D12Resource* texture) = 0;

	virtual void DeleteTextureData(Effekseer::TextureData* textureData) = 0;
};

} // namespace EffekseerRendererDX12

#endif