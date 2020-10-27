
#ifndef __EFFEKSEERRENDERER_DX11_RENDERER_IMPLEMENTED_H__
#define __EFFEKSEERRENDERER_DX11_RENDERER_IMPLEMENTED_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "../../EffekseerRendererCommon/EffekseerRenderer.RenderStateBase.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.StandardRenderer.h"
#include "EffekseerRendererDX11.Base.h"
#include "EffekseerRendererDX11.Renderer.h"

#ifdef _MSC_VER
#include <xmmintrin.h>
#endif

namespace EffekseerRendererDX11
{

using Vertex = EffekseerRenderer::SimpleVertex;
using VertexDistortion = EffekseerRenderer::VertexDistortion;

class OriginalState
	: public ::Effekseer::AlignedAllocationPolicy<16>
{
private:
	std::array<ID3D11SamplerState*, Effekseer::TextureSlotMax> m_samplers;

	ID3D11BlendState* m_blendState;
	std::array<float, Effekseer::TextureSlotMax> m_blendFactor;
	UINT m_blendSampleMask;

	ID3D11DepthStencilState* m_depthStencilState;
	UINT m_depthStencilStateRef;

	ID3D11RasterizerState* m_pRasterizerState;

	ID3D11Buffer* m_vertexConstantBuffer;
	ID3D11Buffer* m_pixelConstantBuffer;

	ID3D11VertexShader* m_pVS;
	ID3D11PixelShader* m_pPS;

	ID3D11InputLayout* m_layout;
	D3D11_PRIMITIVE_TOPOLOGY m_topology;

	std::array<ID3D11ShaderResourceView*, Effekseer::TextureSlotMax> m_psSRVs;

	ID3D11Buffer* m_pVB;
	UINT m_vbStrides;
	UINT m_vbOffset;

	ID3D11Buffer* m_pIB;
	DXGI_FORMAT m_ibFormat;
	UINT m_ibOffset;

public:
	OriginalState();
	~OriginalState();
	void SaveState(ID3D11Device* device, ID3D11DeviceContext* context);
	void LoadState(ID3D11Device* device, ID3D11DeviceContext* context);
	void ReleaseState();
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	描画クラス
	@note
	ツール向けの描画機能。
*/
class RendererImplemented
	: public Renderer,
	  public ::Effekseer::ReferenceObject,
	  public ::Effekseer::AlignedAllocationPolicy<16>
{
	friend class DeviceObject;

private:
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_context;

	VertexBuffer* m_vertexBuffer;
	IndexBuffer* m_indexBuffer;
	IndexBuffer* m_indexBufferForWireframe = nullptr;
	int32_t m_squareMaxCount;

	Shader* m_shader = nullptr;
	Shader* m_shader_distortion = nullptr;
	Shader* m_shader_lighting = nullptr;
	Shader* currentShader = nullptr;

	EffekseerRenderer::StandardRenderer<RendererImplemented, Shader, Vertex, VertexDistortion>* m_standardRenderer;

	// 座標系
	::Effekseer::CoordinateSystem m_coordinateSystem;

	::EffekseerRenderer::RenderStateBase* m_renderState;

	Effekseer::TextureData m_background;

	std::set<DeviceObject*> m_deviceObjects;

	// ステート
	OriginalState* m_state;

	bool m_restorationOfStates;

	D3D11_COMPARISON_FUNC m_depthFunc;

	EffekseerRenderer::DistortingCallback* m_distortingCallback;

public:
	/**
		@brief	コンストラクタ
	*/
	RendererImplemented(int32_t squareMaxCount);

	/**
		@brief	デストラクタ
	*/
	~RendererImplemented();

	void OnLostDevice();
	void OnResetDevice();

	/**
		@brief	初期化
	*/
	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* context, D3D11_COMPARISON_FUNC depthFunc, bool isMSAAEnabled);

	void Destroy();

	void SetRestorationOfStatesFlag(bool flag);

	/**
		@brief	描画開始
	*/
	bool BeginRendering();

	/**
		@brief	描画終了
	*/
	bool EndRendering();

	/**
		@brief	デバイス取得
	*/
	ID3D11Device* GetDevice() override;

	/**
		@brief	コンテキスト取得
	*/
	ID3D11DeviceContext* GetContext() override;

	/**
		@brief	頂点バッファ取得
	*/
	VertexBuffer* GetVertexBuffer();

	/**
		@brief	インデックスバッファ取得
	*/
	IndexBuffer* GetIndexBuffer();

	/**
		@brief	最大描画スプライト数
	*/
	int32_t GetSquareMaxCount() const;

	::EffekseerRenderer::RenderStateBase* GetRenderState();

	/**
		@brief	スプライトレンダラーを生成する。
	*/
	::Effekseer::SpriteRenderer* CreateSpriteRenderer();

	/**
		@brief	リボンレンダラーを生成する。
	*/
	::Effekseer::RibbonRenderer* CreateRibbonRenderer();

	/**
		@brief	リングレンダラーを生成する。
	*/
	::Effekseer::RingRenderer* CreateRingRenderer();

	/**
		@brief	モデルレンダラーを生成する。
	*/
	::Effekseer::ModelRenderer* CreateModelRenderer();

	/**
		@brief	軌跡レンダラーを生成する。
	*/
	::Effekseer::TrackRenderer* CreateTrackRenderer();

	/**
		@brief	テクスチャ読込クラスを生成する。
	*/
	::Effekseer::TextureLoader* CreateTextureLoader(::Effekseer::FileInterface* fileInterface = NULL);

	/**
		@brief	モデル読込クラスを生成する。
	*/
	::Effekseer::ModelLoader* CreateModelLoader(::Effekseer::FileInterface* fileInterface = NULL);

	::Effekseer::MaterialLoader* CreateMaterialLoader(::Effekseer::FileInterface* fileInterface = nullptr) override;

	/**
	@brief	背景を取得する。
	*/
	Effekseer::TextureData* GetBackground() override;

	/**
		@brief	背景を設定する。
	*/
	void SetBackground(ID3D11ShaderResourceView* background) override;

	EffekseerRenderer::DistortingCallback* GetDistortingCallback() override;

	void SetDistortingCallback(EffekseerRenderer::DistortingCallback* callback) override;

	EffekseerRenderer::StandardRenderer<RendererImplemented, Shader, Vertex, VertexDistortion>* GetStandardRenderer()
	{
		return m_standardRenderer;
	}

	void SetVertexBuffer(VertexBuffer* vertexBuffer, int32_t size);
	void SetVertexBuffer(ID3D11Buffer* vertexBuffer, int32_t size);
	void SetIndexBuffer(IndexBuffer* indexBuffer);
	void SetIndexBuffer(ID3D11Buffer* indexBuffer);

	void SetLayout(Shader* shader);
	void DrawSprites(int32_t spriteCount, int32_t vertexOffset);
	void DrawPolygon(int32_t vertexCount, int32_t indexCount);

	Shader* GetShader(bool useTexture, ::Effekseer::RendererMaterialType materialType) const;
	void BeginShader(Shader* shader);
	void EndShader(Shader* shader);

	void SetVertexBufferToShader(const void* data, int32_t size, int32_t dstOffset);

	void SetPixelBufferToShader(const void* data, int32_t size, int32_t dstOffset);

	void SetTextures(Shader* shader, Effekseer::TextureData** textures, int32_t count);

	void ResetRenderState();

	Effekseer::TextureData* CreateProxyTexture(EffekseerRenderer::ProxyTextureType type) override;

	void DeleteProxyTexture(Effekseer::TextureData* data) override;

	virtual int GetRef()
	{
		return ::Effekseer::ReferenceObject::GetRef();
	}
	virtual int AddRef()
	{
		return ::Effekseer::ReferenceObject::AddRef();
	}
	virtual int Release()
	{
		return ::Effekseer::ReferenceObject::Release();
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRendererDX11
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_DX11_RENDERER_IMPLEMENTED_H__