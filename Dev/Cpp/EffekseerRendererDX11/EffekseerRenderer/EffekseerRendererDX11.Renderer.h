
#ifndef	__EFFEKSEERRENDERER_DX11_RENDERER_H__
#define	__EFFEKSEERRENDERER_DX11_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererDX11.Base.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.Renderer.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace EffekseerRendererDX11
{
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
	Renderer() {}
	virtual ~Renderer() {}

public:
	/**
		@brief	インスタンスを生成する。
		@param	device	[in]	DirectXのデバイス
		@param	squareMaxCount	[in]	最大描画スプライト数
		@return	インスタンス
	*/
	static Renderer* Create( ID3D11Device* device, ID3D11DeviceContext* context, int32_t squareMaxCount );

	/**
		@brief	デバイスを取得する。
	*/
	virtual ID3D11Device* GetDevice() = 0;

	/**
	@brief	背景を取得する。
	*/
	virtual ID3D11ShaderResourceView* GetBackground() = 0;

	/**
	@brief	背景を設定する。
	*/
	virtual void SetBackground(ID3D11ShaderResourceView* background) = 0;
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

public:
	ID3D11Buffer*		VertexBuffer;
	ID3D11Buffer*		IndexBuffer;
	int32_t				VertexCount;
	int32_t				IndexCount;
	int32_t				FaceCount;
	int32_t				ModelCount;

	Model( uint8_t* data, int32_t size )
		: Effekseer::Model	( data, size )
		, VertexBuffer	( NULL )
		, IndexBuffer	( NULL )
		, VertexCount		( 0 )
		, IndexCount		( 0 )
		, FaceCount			( 0 )
		, ModelCount		( 0 )
	{
	}

	virtual ~Model()
	{
		ES_SAFE_RELEASE( VertexBuffer );
		ES_SAFE_RELEASE( IndexBuffer );
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