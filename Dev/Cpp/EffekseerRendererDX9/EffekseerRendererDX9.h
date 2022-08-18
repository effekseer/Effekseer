
#ifndef __EFFEKSEERRENDERER_DX9_BASE_PRE_H__
#define __EFFEKSEERRENDERER_DX9_BASE_PRE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <Effekseer.h>

#include <d3d9.h>
#include <windows.h>

#if _WIN32
#pragma comment(lib, "d3d9.lib")
#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace EffekseerRendererDX9
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class Renderer;

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRendererDX9
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_DX9_PRE_BASE_H__

#ifndef __EFFEKSEERRENDERER_RENDERER_H__
#define __EFFEKSEERRENDERER_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <Effekseer.h>

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------

namespace Effekseer
{
namespace Backend
{
class VertexBuffer;
class IndexBuffer;
class GraphicsDevice;
} // namespace Backend
} // namespace Effekseer

namespace EffekseerRenderer
{

class Renderer;
using RendererRef = ::Effekseer::RefPtr<Renderer>;

/**
	@brief	Specify a shader for renderer from external class
	@note
	For Effekseer tools
*/
struct ExternalShaderSettings
{
	Effekseer::Backend::ShaderRef StandardShader;
	Effekseer::Backend::ShaderRef ModelShader;
	Effekseer::AlphaBlendType Blend;
};

/**
	@brief	
	\~english A callback to distort a background before drawing
	\~japanese 背景を歪ませるエフェクトを描画する前に実行されるコールバック
	
*/
class DistortingCallback
{
public:
	DistortingCallback()
	{
	}
	virtual ~DistortingCallback()
	{
	}

	/**
	@brief	
	\~english A callback
	\~japanese コールバック
	@note
	\~english Don't hold renderer in the instance
	\~japanese インスタンス内にrendererを保持してはいけない
	*/
	virtual bool OnDistorting(Renderer* renderer)
	{
		return false;
	}
};

/**
	@brief
	\~english A status of UV when particles are rendered.
	\~japanese パーティクルを描画する時のUVの状態
*/
enum class UVStyle
{
	Normal,
	VerticalFlipped,
};

/**
	@brief
	\~english A type of texture which is rendered when textures are not assigned.
	\~japanese テクスチャが設定されていないときに描画されるテクスチャの種類
*/
enum class ProxyTextureType
{
	White,
	Normal,
};

class CommandList : public ::Effekseer::IReference
{
public:
	CommandList() = default;
	virtual ~CommandList() = default;
};

class SingleFrameMemoryPool : public ::Effekseer::IReference
{
public:
	SingleFrameMemoryPool() = default;
	virtual ~SingleFrameMemoryPool() = default;

	/**
		@brief
		\~English	notify that new frame is started.
		\~Japanese	新規フレームが始ったことを通知する。
	*/
	virtual void NewFrame()
	{
	}
};

struct DepthReconstructionParameter
{
	float DepthBufferScale = 1.0f;
	float DepthBufferOffset = 0.0f;
	float ProjectionMatrix33 = 0.0f;
	float ProjectionMatrix34 = 0.0f;
	float ProjectionMatrix43 = 0.0f;
	float ProjectionMatrix44 = 0.0f;
};

::Effekseer::ModelLoaderRef CreateModelLoader(::Effekseer::Backend::GraphicsDeviceRef gprahicsDevice, ::Effekseer::FileInterfaceRef fileInterface = nullptr);

class Renderer : public ::Effekseer::IReference
{
protected:
	Renderer();
	virtual ~Renderer();

	class Impl;
	std::unique_ptr<Impl> impl;

public:
	/**
		@brief	only for Effekseer backend developer. Effekseer User doesn't need it.
	*/
	Impl* GetImpl();

	/**
		@brief	デバイスロストが発生した時に実行する。
	*/
	virtual void OnLostDevice() = 0;

	/**
		@brief	デバイスがリセットされた時に実行する。
	*/
	virtual void OnResetDevice() = 0;

	/**
		@brief	ステートを復帰するかどうかのフラグを設定する。
	*/
	virtual void SetRestorationOfStatesFlag(bool flag) = 0;

	/**
		@brief	描画を開始する時に実行する。
	*/
	virtual bool BeginRendering() = 0;

	/**
		@brief	描画を終了する時に実行する。
	*/
	virtual bool EndRendering() = 0;

	/**
		@brief	Get the direction of light
	*/
	virtual ::Effekseer::Vector3D GetLightDirection() const;

	/**
		@brief	Specifiy the direction of light
	*/
	virtual void SetLightDirection(const ::Effekseer::Vector3D& direction);

	/**
		@brief	Get the color of light
	*/
	virtual const ::Effekseer::Color& GetLightColor() const;

	/**
		@brief	Specify the color of light
	*/
	virtual void SetLightColor(const ::Effekseer::Color& color);

	/**
		@brief	Get the color of ambient
	*/
	virtual const ::Effekseer::Color& GetLightAmbientColor() const;

	/**
		@brief	Specify the color of ambient
	*/
	virtual void SetLightAmbientColor(const ::Effekseer::Color& color);

	/**
		@brief	最大描画スプライト数を取得する。
	*/
	virtual int32_t GetSquareMaxCount() const = 0;

	/**
		@brief	Get a projection matrix
	*/
	virtual ::Effekseer::Matrix44 GetProjectionMatrix() const;

	/**
		@brief	Set a projection matrix
	*/
	virtual void SetProjectionMatrix(const ::Effekseer::Matrix44& mat);

	/**
		@brief	Get a camera matrix
	*/
	virtual ::Effekseer::Matrix44 GetCameraMatrix() const;

	/**
		@brief	Set a camera matrix
	*/
	virtual void SetCameraMatrix(const ::Effekseer::Matrix44& mat);

	/**
		@brief	Get a camera projection matrix
	*/
	virtual ::Effekseer::Matrix44 GetCameraProjectionMatrix() const;

	/**
		@brief	Get a front direction of camera
		@note
		We don't recommend to use it without understanding of internal code.
	*/
	virtual ::Effekseer::Vector3D GetCameraFrontDirection() const;

	/**
		@brief	Get a position of camera
		@note
		We don't recommend to use it without understanding of internal code.
	*/
	virtual ::Effekseer::Vector3D GetCameraPosition() const;

	/**
		@brief	Set a front direction and position of camera manually
		@param front (Right Hand) a direction from focus to eye, (Left Hand) a direction from eye to focus,
		@note
		These are set based on camera matrix automatically.
		It is failed on some platform.
	*/
	virtual void SetCameraParameter(const ::Effekseer::Vector3D& front, const ::Effekseer::Vector3D& position);

	/**
		@brief	スプライトレンダラーを生成する。
	*/
	virtual ::Effekseer::SpriteRendererRef CreateSpriteRenderer() = 0;

	/**
		@brief	リボンレンダラーを生成する。
	*/
	virtual ::Effekseer::RibbonRendererRef CreateRibbonRenderer() = 0;

	/**
		@brief	リングレンダラーを生成する。
	*/
	virtual ::Effekseer::RingRendererRef CreateRingRenderer() = 0;

	/**
		@brief	モデルレンダラーを生成する。
	*/
	virtual ::Effekseer::ModelRendererRef CreateModelRenderer() = 0;

	/**
		@brief	軌跡レンダラーを生成する。
	*/
	virtual ::Effekseer::TrackRendererRef CreateTrackRenderer() = 0;

	/**
		@brief	標準のテクスチャ読込クラスを生成する。
	*/
	virtual ::Effekseer::TextureLoaderRef CreateTextureLoader(::Effekseer::FileInterfaceRef fileInterface = nullptr) = 0;

	/**
		@brief	標準のモデル読込クラスを生成する。
	*/
	virtual ::Effekseer::ModelLoaderRef CreateModelLoader(::Effekseer::FileInterfaceRef fileInterface = nullptr) = 0;

	/**
	@brief
	\~english Create default material loader
	\~japanese 標準のマテリアル読込クラスを生成する。

	*/
	virtual ::Effekseer::MaterialLoaderRef CreateMaterialLoader(::Effekseer::FileInterfaceRef fileInterface = nullptr) = 0;

	/**
		@brief	レンダーステートを強制的にリセットする。
	*/
	virtual void ResetRenderState() = 0;

	/**
	@brief	背景を歪ませるエフェクトが描画される前に呼ばれるコールバックを取得する。
	*/
	virtual DistortingCallback* GetDistortingCallback() = 0;

	/**
	@brief	背景を歪ませるエフェクトが描画される前に呼ばれるコールバックを設定する。
	*/
	virtual void SetDistortingCallback(DistortingCallback* callback) = 0;

	/**
	@brief
	\~english Get draw call count
	\~japanese ドローコールの回数を取得する
	*/
	virtual int32_t GetDrawCallCount() const;

	/**
	@brief
	\~english Get the number of vertex drawn
	\~japanese 描画された頂点数をリセットする
	*/
	virtual int32_t GetDrawVertexCount() const;

	/**
	@brief
	\~english Reset draw call count
	\~japanese ドローコールの回数をリセットする
	*/
	virtual void ResetDrawCallCount();

	/**
	@brief
	\~english Reset the number of vertex drawn
	\~japanese 描画された頂点数をリセットする
	*/
	virtual void ResetDrawVertexCount();

	/**
	@brief
	\~english Get a render mode.
	\~japanese 描画モードを取得する。
	*/
	virtual Effekseer::RenderMode GetRenderMode() const;

	/**
	@brief
	\~english Specify a render mode.
	\~japanese 描画モードを設定する。
	*/
	virtual void SetRenderMode(Effekseer::RenderMode renderMode);

	/**
	@brief
	\~english Get an UV Style of texture when particles are rendered.
	\~japanese パーティクルを描画するときのUVの状態を取得する。
	*/
	virtual UVStyle GetTextureUVStyle() const;

	/**
	@brief
	\~english Set an UV Style of texture when particles are rendered.
	\~japanese パーティクルを描画するときのUVの状態を設定する。
	*/
	virtual void SetTextureUVStyle(UVStyle style);

	/**
	@brief
	\~english Get an UV Style of background when particles are rendered.
	\~japanese パーティクルを描画するときの背景のUVの状態を取得する。
	*/
	virtual UVStyle GetBackgroundTextureUVStyle() const;

	/**
	@brief
	\~english Set an UV Style of background when particles are rendered.
	\~japanese パーティクルを描画するときの背景のUVの状態を設定する。
	*/
	virtual void SetBackgroundTextureUVStyle(UVStyle style);

	/**
	@brief
	\~english Get a current time (s)
	\~japanese 現在の時間を取得する。(秒)
	*/
	virtual float GetTime() const;

	/**
	@brief
	\~english Set a current time (s)
	\~japanese 現在の時間を設定する。(秒)
	*/
	virtual void SetTime(float time);

	/**
	@brief
	\~English	specify a command list to render.  This function is available except DirectX9, DirectX11 and OpenGL.
	\~Japanese	描画に使用するコマンドリストを設定する。この関数はDirectX9、DirectX11、OpenGL以外で使用できる。
	*/
	virtual void SetCommandList(Effekseer::RefPtr<CommandList> commandList)
	{
	}

	/**
		@brief	\~English	Get a background texture.
		\~Japanese	背景を取得する。
		@note
		\~English	Textures are generated by a function specific to each backend or SetBackground.
		\~Japanese	テクスチャは各バックエンド固有の関数かSetBackgroundで生成される。
	*/
	virtual const ::Effekseer::Backend::TextureRef& GetBackground();

	/**
	@brief
	\~English	Specify a background texture.
	\~Japanese	背景のテクスチャを設定する。
	*/
	virtual void SetBackground(::Effekseer::Backend::TextureRef texture);

	/**
	@brief
	\~English	Create a proxy texture
	\~Japanese	代替のテクスチャを生成する
	*/
	virtual ::Effekseer::Backend::TextureRef CreateProxyTexture(ProxyTextureType type);

	/**
	@brief
	\~English	Delete a proxy texture
	\~Japanese	代替のテクスチャを削除する
	*/
	virtual void DeleteProxyTexture(Effekseer::Backend::TextureRef& texture);

	/**
		@brief	
		\~English	Get a depth texture and parameters to reconstruct from z to depth
		\~Japanese	深度画像とZから深度を復元するためのパラメーターを取得する。
	*/
	virtual void GetDepth(::Effekseer::Backend::TextureRef& texture, DepthReconstructionParameter& reconstructionParam);

	/**
		@brief	
		\~English	Specify a depth texture and parameters to reconstruct from z to depth
		\~Japanese	深度画像とZから深度を復元するためのパラメーターを設定する。
	*/
	virtual void SetDepth(::Effekseer::Backend::TextureRef texture, const DepthReconstructionParameter& reconstructionParam);

	/**
		@brief
		\~English	Specify whether maintain gamma color in a linear color space
		\~Japanese	リニア空間でもガンマカラーを維持するようにするか、を設定する。

	*/
	virtual void SetMaintainGammaColorInLinearColorSpace(bool value);

	/**
		@brief	
		\~English	Get the graphics device
		\~Japanese	グラフィクスデバイスを取得する。
	*/
	virtual Effekseer::Backend::GraphicsDeviceRef GetGraphicsDevice() const;

	/**
		@brief	Get external shader settings
		@note
		For	Effekseer tools
	*/
	virtual std::shared_ptr<ExternalShaderSettings> GetExternalShaderSettings() const;

	/**
		@brief	Specify external shader settings
		@note
		For	Effekseer tools
	*/
	virtual void SetExternalShaderSettings(const std::shared_ptr<ExternalShaderSettings>& settings);
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRenderer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_RENDERER_H__
#ifndef __EFFEKSEERRENDERER_TEXTURELOADER_H__
#define __EFFEKSEERRENDERER_TEXTURELOADER_H__

#include <Effekseer.h>

namespace EffekseerRenderer
{

::Effekseer::TextureLoaderRef CreateTextureLoader(::Effekseer::Backend::GraphicsDeviceRef gprahicsDevice,
												  ::Effekseer::FileInterfaceRef fileInterface = nullptr,
												  ::Effekseer::ColorSpaceType colorSpaceType = ::Effekseer::ColorSpaceType::Gamma);

} // namespace EffekseerRenderer

#endif // __EFFEKSEERRENDERER_TEXTURELOADER_H__

#ifndef __EFFEKSEERRENDERER_DX9_RENDERER_H__
#define __EFFEKSEERRENDERER_DX9_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace EffekseerRendererDX9
{

::Effekseer::Backend::GraphicsDeviceRef CreateGraphicsDevice(LPDIRECT3DDEVICE9 device);

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
