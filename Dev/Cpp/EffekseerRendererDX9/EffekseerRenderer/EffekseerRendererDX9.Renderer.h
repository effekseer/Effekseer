
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

/**
@brief	テクスチャ読込クラスを生成する。
*/
::Effekseer::TextureLoader* CreateTextureLoader(LPDIRECT3DDEVICE9 device, ::Effekseer::FileInterface* fileInterface = NULL);

/**
@brief	モデル読込クラスを生成する。
*/
::Effekseer::ModelLoader* CreateModelLoader(LPDIRECT3DDEVICE9 device, ::Effekseer::FileInterface* fileInterface = NULL);

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
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
		@param	device	[in]	DirectXのデバイス
		@param	squareMaxCount	[in]	最大描画スプライト数
		@return	インスタンス
	*/
	static Renderer* Create(LPDIRECT3DDEVICE9 device, int32_t squareMaxCount);

	/**
		@brief	デバイスを取得する。
	*/
	virtual LPDIRECT3DDEVICE9 GetDevice() = 0;

	/**
		@brief	デバイスロストリセット間でデバイス自体を再構築する際に外部からデバイスを設定する。
	*/
	virtual void ChangeDevice(LPDIRECT3DDEVICE9 device) = 0;

	/**
	@brief	背景を取得する。
	*/
	virtual Effekseer::TextureData* GetBackground() = 0;

	/**
	@brief	背景を設定する。
	*/
	virtual void SetBackground(IDirect3DTexture9* background) = 0;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	モデル
*/
class Model : public Effekseer::Model
{
private:
	LPDIRECT3DDEVICE9 device_ = nullptr;

public:
	struct InternalModel
	{
		IDirect3DVertexBuffer9* VertexBuffer;
		IDirect3DIndexBuffer9* IndexBuffer;
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

	Model(uint8_t* data, int32_t size, LPDIRECT3DDEVICE9 device)
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
} // namespace EffekseerRendererDX9
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_DX9_RENDERER_H__