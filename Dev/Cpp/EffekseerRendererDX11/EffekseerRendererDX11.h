﻿
#ifndef	__EFFEKSEERRENDERER_DX11_BASE_PRE_H__
#define	__EFFEKSEERRENDERER_DX11_BASE_PRE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <Effekseer.h>

#include <windows.h>
#include <d3d11.h>

#if _WIN32
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "d3d11.lib" )
#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace EffekseerRendererDX11
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class Renderer;

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRENDERER_DX11_PRE_BASE_H__

#ifndef	__EFFEKSEERRENDERER_RENDERER_H__
#define	__EFFEKSEERRENDERER_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <Effekseer.h>

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------

/**
	@brief	背景を歪ませるエフェクトを描画する前に実行されるコールバック
*/
class DistortingCallback
{
public:
	DistortingCallback() {}
	virtual ~DistortingCallback() {}

	virtual bool OnDistorting() { return false; }
};

struct DynamicVertex
{
	::Effekseer::Vector3D Pos;
	::Effekseer::Color Col;
	//! packed vector
	::Effekseer::Color Normal;
	//! packed vector
	::Effekseer::Color Tangent;

	union
	{
		//! UV1 (for template)
		float UV[2];
		float UV1[2];
	};

	float UV2[2];
};

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------

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
	virtual void NewFrame() {}
};

class Renderer
	: public ::Effekseer::IReference
{
protected:
	Renderer();
	virtual ~Renderer();

	class Impl;
	Impl* impl = nullptr;

public:
	/**
		@brief	デバイスロストが発生した時に実行する。
	*/
	virtual void OnLostDevice() = 0;

	/**
		@brief	デバイスがリセットされた時に実行する。
	*/
	virtual void OnResetDevice() = 0;

	/**
		@brief	このインスタンスを破棄する。
	*/
	virtual void Destroy() = 0;

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
		@brief	ライトの方向を取得する。
	*/
	virtual const ::Effekseer::Vector3D& GetLightDirection() const = 0;

	/**
		@brief	ライトの方向を設定する。
	*/
	virtual void SetLightDirection( const ::Effekseer::Vector3D& direction ) = 0;

	/**
		@brief	ライトの色を取得する。
	*/
	virtual const ::Effekseer::Color& GetLightColor() const = 0;

	/**
		@brief	ライトの色を設定する。
	*/
	virtual void SetLightColor( const ::Effekseer::Color& color ) = 0;

	/**
		@brief	ライトの環境光の色を取得する。
	*/
	virtual const ::Effekseer::Color& GetLightAmbientColor() const = 0;

	/**
		@brief	ライトの環境光の色を設定する。
	*/
	virtual void SetLightAmbientColor( const ::Effekseer::Color& color ) = 0;

		/**
		@brief	最大描画スプライト数を取得する。
	*/
	virtual int32_t GetSquareMaxCount() const = 0;

	/**
		@brief	投影行列を取得する。
	*/
	virtual const ::Effekseer::Matrix44& GetProjectionMatrix() const = 0;

	/**
		@brief	投影行列を設定する。
	*/
	virtual void SetProjectionMatrix( const ::Effekseer::Matrix44& mat ) = 0;

	/**
		@brief	カメラ行列を取得する。
	*/
	virtual const ::Effekseer::Matrix44& GetCameraMatrix() const = 0;

	/**
		@brief	カメラ行列を設定する。
	*/
	virtual void SetCameraMatrix( const ::Effekseer::Matrix44& mat ) = 0;

	/**
		@brief	カメラプロジェクション行列を取得する。
	*/
	virtual ::Effekseer::Matrix44& GetCameraProjectionMatrix() = 0;

	/**
		@brief	Get a front direction of camera
	*/
	virtual ::Effekseer::Vector3D GetCameraFrontDirection() const = 0;

	/**
		@brief	Get a position of camera
	*/
	virtual ::Effekseer::Vector3D GetCameraPosition() const = 0;

	/**
		@brief	Set a front direction and position of camera manually
		@note
		These are set based on camera matrix automatically.
		It is failed on some platform.
	*/
	virtual void SetCameraParameter(const ::Effekseer::Vector3D& front, const ::Effekseer::Vector3D& position) = 0;

	/**
		@brief	スプライトレンダラーを生成する。
	*/
	virtual ::Effekseer::SpriteRenderer* CreateSpriteRenderer() = 0;

	/**
		@brief	リボンレンダラーを生成する。
	*/
	virtual ::Effekseer::RibbonRenderer* CreateRibbonRenderer() = 0;

	/**
		@brief	リングレンダラーを生成する。
	*/
	virtual ::Effekseer::RingRenderer* CreateRingRenderer() = 0;

	/**
		@brief	モデルレンダラーを生成する。
	*/
	virtual ::Effekseer::ModelRenderer* CreateModelRenderer() = 0;

	/**
		@brief	軌跡レンダラーを生成する。
	*/
	virtual ::Effekseer::TrackRenderer* CreateTrackRenderer() = 0;

	/**
		@brief	標準のテクスチャ読込クラスを生成する。
	*/
	virtual ::Effekseer::TextureLoader* CreateTextureLoader( ::Effekseer::FileInterface* fileInterface = NULL ) = 0;

	/**
		@brief	標準のモデル読込クラスを生成する。
	*/
	virtual ::Effekseer::ModelLoader* CreateModelLoader( ::Effekseer::FileInterface* fileInterface = NULL ) = 0;

	/**
	@brief	
	\~english Create default material loader
	\~japanese 標準のマテリアル読込クラスを生成する。

	*/
	virtual ::Effekseer::MaterialLoader* CreateMaterialLoader(::Effekseer::FileInterface* fileInterface = nullptr) = 0;

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
	@brief	描画モードを設定する。
	*/
	virtual void SetRenderMode( Effekseer::RenderMode renderMode ) = 0;

	/**
	@brief	描画モードを取得する。
	*/
	virtual Effekseer::RenderMode GetRenderMode() = 0;

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
	virtual void SetCommandList(CommandList* commandList) {}

	/**
	@brief
	\~English	Specify a background texture.
	\~Japanese	背景のテクスチャを設定する。
	@note
	\~English	Specified texture is not deleted by the renderer. This function is available except DirectX9, DirectX11 and OpenGL.
	\~Japanese	設定されたテクスチャはレンダラーによって削除されない。この関数はDirectX9、DirectX11、OpenGL以外で使用できる。
	*/
	virtual void SetBackgroundTexture(::Effekseer::TextureData* textureData);
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRENDERER_RENDERER_H__

#ifndef	__EFFEKSEERRENDERER_DX11_RENDERER_H__
#define	__EFFEKSEERRENDERER_DX11_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace EffekseerRendererDX11
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

/**
@brief	テクスチャ読込クラスを生成する。
*/
::Effekseer::TextureLoader* CreateTextureLoader(ID3D11Device* device, ID3D11DeviceContext* context, ::Effekseer::FileInterface* fileInterface = NULL);

/**
@brief	モデル読込クラスを生成する。
*/
::Effekseer::ModelLoader* CreateModelLoader(ID3D11Device* device, ::Effekseer::FileInterface* fileInterface = NULL);


/**
	@brief	描画クラス
*/
class Renderer
	: public ::EffekseerRenderer::Renderer
{
protected:
	Renderer() {}
	virtual ~Renderer() {}

public:
	/**
		@brief	インスタンスを生成する。
		@param	device		DirectXのデバイス
		@param	context		DirectXのコンテキスト
		@param	squareMaxCount	最大描画スプライト数
		@param	depthFunc	奥行きの計算方法
		@return	インスタンス
	*/
	static Renderer* Create(
		ID3D11Device* device, 
		ID3D11DeviceContext* context, 
		int32_t squareMaxCount, 
		D3D11_COMPARISON_FUNC depthFunc = D3D11_COMPARISON_LESS);

	virtual ID3D11Device* GetDevice() = 0;

	virtual ID3D11DeviceContext* GetContext() = 0;

	/**
		@brief	\~English	Get background
				\~Japanese	背景を取得する
	*/
	virtual Effekseer::TextureData* GetBackground() = 0;

	/**
		@brief	\~English	Set background
				\~Japanese	背景を設定する
	*/
	virtual void SetBackground(ID3D11ShaderResourceView* background) = 0;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
@brief	\~English	Model
		\~Japanese	モデル
*/
class Model 
	: public Effekseer::Model
{
private:

public:

	struct InternalModel
	{
		ID3D11Buffer*		VertexBuffer;
		ID3D11Buffer*		IndexBuffer;
		int32_t				VertexCount;
		int32_t				IndexCount;
		int32_t				FaceCount;

		InternalModel()
		{
			VertexBuffer = nullptr;
			IndexBuffer = nullptr;
			VertexCount = 0;
			IndexCount = 0;
			FaceCount = 0;
		}

		virtual ~InternalModel()
		{
			ES_SAFE_RELEASE(VertexBuffer);
			ES_SAFE_RELEASE(IndexBuffer);
		}
	};

	InternalModel*				InternalModels = nullptr;
	int32_t						ModelCount;

	Model( uint8_t* data, int32_t size )
		: Effekseer::Model	( data, size )
		, InternalModels(nullptr)
		, ModelCount(0)
	{
		this->m_vertexSize = sizeof(VertexWithIndex);
	}

	virtual ~Model()
	{
		ES_SAFE_DELETE_ARRAY(InternalModels);
	}
};


//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRENDERER_DX11_RENDERER_H__