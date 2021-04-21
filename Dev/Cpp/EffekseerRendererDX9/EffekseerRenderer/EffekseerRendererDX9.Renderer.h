
#ifndef __EFFEKSEERRENDERER_DX9_RENDERER_H__
#define __EFFEKSEERRENDERER_DX9_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "../../EffekseerRendererCommon/EffekseerRenderer.Renderer.h"
#include "EffekseerRendererDX9.Base.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace EffekseerRendererDX9
{

::Effekseer::Backend::GraphicsDeviceRef CreateGraphicsDevice(LPDIRECT3DDEVICE9 device);

[[deprecated("please use EffekseerRenderer::CreateTextureLoader")]] ::Effekseer::TextureLoaderRef CreateTextureLoader(
	Effekseer::Backend::GraphicsDeviceRef graphicsDevice,
	::Effekseer::FileInterface* fileInterface = nullptr,
	::Effekseer::ColorSpaceType colorSpaceType = ::Effekseer::ColorSpaceType::Gamma);

[[deprecated("please use EffekseerRenderer::CreateTextureLoader")]] ::Effekseer::ModelLoaderRef CreateModelLoader(Effekseer::Backend::GraphicsDeviceRef graphicsDevice, ::Effekseer::FileInterface* fileInterface = nullptr);

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	描画クラス
*/
class Renderer;
using RendererRef = ::Effekseer::RefPtr<Renderer>;

class Renderer : public ::EffekseerRenderer::Renderer
{
protected:
	Renderer()
	{
	}
	virtual ~Renderer()
	{
	}

public:
	/**
	@brief	Create an instance
	@param	graphicsDevice	Effekseer graphics device
	@param	squareMaxCount	The number of maximum sprites
	@return	instance
*/
	static RendererRef Create(Effekseer::Backend::GraphicsDeviceRef graphicsDevice, int32_t squareMaxCount);

	/**
		@brief	インスタンスを生成する。
		@param	device	[in]	DirectXのデバイス
		@param	squareMaxCount	[in]	最大描画スプライト数
		@return	インスタンス
	*/
	static RendererRef Create(LPDIRECT3DDEVICE9 device, int32_t squareMaxCount);

	/**
		@brief	デバイスを取得する。
	*/
	virtual LPDIRECT3DDEVICE9 GetDevice() = 0;

	/**
		@brief	デバイスロストリセット間でデバイス自体を再構築する際に外部からデバイスを設定する。
	*/
	virtual void ChangeDevice(LPDIRECT3DDEVICE9 device) = 0;

	/**
	@brief	背景を設定する。
	*/
	virtual void SetBackground(IDirect3DTexture9* background) = 0;
};

} // namespace EffekseerRendererDX9
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_DX9_RENDERER_H__