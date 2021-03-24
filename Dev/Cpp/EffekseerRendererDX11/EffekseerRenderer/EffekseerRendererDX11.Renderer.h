
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

[[deprecated("please use EffekseerRenderer::CreateTextureLoader")]] ::Effekseer::TextureLoaderRef CreateTextureLoader(::Effekseer::Backend::GraphicsDeviceRef gprahicsDevice,
																													  ::Effekseer::FileInterface* fileInterface = nullptr,
																													  ::Effekseer::ColorSpaceType colorSpaceType = ::Effekseer::ColorSpaceType::Gamma);

[[deprecated("please use EffekseerRenderer::CreateModelLoader")]] ::Effekseer::ModelLoaderRef CreateModelLoader(::Effekseer::Backend::GraphicsDeviceRef gprahicsDevice, ::Effekseer::FileInterface* fileInterface = nullptr);

::Effekseer::Backend::TextureRef CreateTexture(::Effekseer::Backend::GraphicsDeviceRef gprahicsDevice, ID3D11ShaderResourceView* srv, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv);

/**
		@brief	\~English	Properties in a texture
				\~Japanese	テクスチャ内のプロパティ

		@note	\~English	You need not to release pointers
				\~Japanese	ポインタの解放する必要はない
*/
struct TextureProperty
{
	ID3D11ShaderResourceView* ShaderResourceViewPtr = nullptr;
	ID3D11RenderTargetView* RenderTargetViewPtr = nullptr;
	ID3D11DepthStencilView* DepthStencilViewPtr = nullptr;
};

TextureProperty GetTextureProperty(::Effekseer::Backend::TextureRef texture);

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
		@brief
		\~english	Create an instance
		\~japanese	インスタンスを生成する。
		@param	device		Device of DirectX
		@param	context		Context of DirectX
		@param	squareMaxCount
		\~english	the number of maximum sprites
		\~japanese	最大描画スプライト数
		@param	depthFunc	a func to compare a dpeth
		@param	isMSAAEnabled whether is MSAA enabled
		@return	instance
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
	[[deprecated("please use EffekseerRenderer::SetBackground")]] virtual void SetBackground(ID3D11ShaderResourceView* background) = 0;

	/**
		@brief	\~English	Reset internal states for DefferedContext
				\~Japanese	DefferedContextのために、内部の状態をリセットする。
		@note	\~English	Please call before BeginRendering after FinishCommandList
				\~Japanese	FinishCommandListの後でBeginRenderingの前に呼んでください。
	*/
	virtual void ResetStateForDefferedContext() = 0;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRendererDX11
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_DX11_RENDERER_H__