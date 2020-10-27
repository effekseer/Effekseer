
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

/**
@brief	テクスチャ読込クラスを生成する。
*/
::Effekseer::TextureLoader* CreateTextureLoader(ID3D11Device* device,
												ID3D11DeviceContext* context,
												::Effekseer::FileInterface* fileInterface = nullptr,
												::Effekseer::ColorSpaceType colorSpaceType = ::Effekseer::ColorSpaceType::Gamma);

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
	static Renderer* Create(
		ID3D11Device* device,
		ID3D11DeviceContext* context,
		int32_t squareMaxCount,
		D3D11_COMPARISON_FUNC depthFunc = D3D11_COMPARISON_LESS_EQUAL,
		bool isMSAAEnabled = false);

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
	ID3D11Device* device_ = nullptr;

public:
	struct InternalModel
	{
		ID3D11Buffer* VertexBuffer;
		ID3D11Buffer* IndexBuffer;
		int32_t VertexCount;
		int32_t IndexCount;
		int32_t FaceCount;

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

	InternalModel* InternalModels = nullptr;
	int32_t ModelCount;
	bool IsLoadedOnGPU = false;

	Model(uint8_t* data, int32_t size, ID3D11Device* device)
		: Effekseer::Model(data, size)
		, device_(device)
		, InternalModels(nullptr)
		, ModelCount(0)
	{
		this->m_vertexSize = sizeof(VertexWithIndex);
		ES_SAFE_ADDREF(device_);
	}

	virtual ~Model()
	{
		ES_SAFE_DELETE_ARRAY(InternalModels);
		ES_SAFE_RELEASE(device_);
	}

	bool LoadToGPU();
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRendererDX11
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_DX11_RENDERER_H__