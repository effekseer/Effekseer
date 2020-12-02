
#ifndef __EFFEKSEERRENDERER_DX11_RENDERER_H__
#define __EFFEKSEERRENDERER_DX11_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "../../EffekseerRendererCommon/EffekseerRenderer.Renderer.h"
#include "EffekseerRendererDX11.Base.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace EffekseerRendererDX11
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

::Effekseer::Backend::GraphicsDeviceRef CreateGraphicsDevice(ID3D11Device* device,
														   ID3D11DeviceContext* context);

::Effekseer::TextureLoaderRef CreateTextureLoader(::Effekseer::Backend::GraphicsDeviceRef gprahicsDevice,
												::Effekseer::FileInterface* fileInterface = nullptr,
												::Effekseer::ColorSpaceType colorSpaceType = ::Effekseer::ColorSpaceType::Gamma);

::Effekseer::ModelLoaderRef CreateModelLoader(::Effekseer::Backend::GraphicsDeviceRef gprahicsDevice, ::Effekseer::FileInterface* fileInterface = nullptr);

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
		@brief	インスタンスを生成する。
		@param	device		DirectXのデバイス
		@param	context		DirectXのコンテキスト
		@param	squareMaxCount	最大描画スプライト数
		@param	depthFunc	奥行きの計算方法
		@param	isMSAAEnabled whether is MSAA enabled
		@return	インスタンス
	*/
	static RendererRef Create(ID3D11Device* device,
							ID3D11DeviceContext* context,
							int32_t squareMaxCount,
							D3D11_COMPARISON_FUNC depthFunc = D3D11_COMPARISON_LESS_EQUAL,
							bool isMSAAEnabled = false);

	virtual ID3D11Device* GetDevice() = 0;

	virtual ID3D11DeviceContext* GetContext() = 0;

	/**
		@brief	\~English	Set background
				\~Japanese	背景を設定する
	*/
	virtual void SetBackground(ID3D11ShaderResourceView* background) = 0;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRendererDX11
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_DX11_RENDERER_H__