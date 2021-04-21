
#ifndef __EFFEKSEERRENDERER_DX9_RENDERER_IMPLEMENTED_H__
#define __EFFEKSEERRENDERER_DX9_RENDERER_IMPLEMENTED_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "../../EffekseerRendererCommon/EffekseerRenderer.RenderStateBase.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.StandardRenderer.h"
#include "EffekseerRendererDX9.Base.h"
#include "EffekseerRendererDX9.Renderer.h"
#include "GraphicsDevice.h"
#include <array>

namespace EffekseerRendererDX9
{

using Vertex = EffekseerRenderer::SimpleVertex;

class RendererImplemented;
using RendererImplementedRef = ::Effekseer::RefPtr<RendererImplemented>;

class RendererImplemented : public Renderer, public ::Effekseer::ReferenceObject
{
	friend class DeviceObject;

private:
	VertexBuffer* m_vertexBuffer;
	IndexBuffer* m_indexBuffer = nullptr;
	IndexBuffer* m_indexBufferForWireframe = nullptr;
	int32_t m_squareMaxCount;

	Shader* shader_unlit_ = nullptr;
	Shader* shader_distortion_ = nullptr;
	Shader* shader_lit_ = nullptr;
	Shader* shader_ad_unlit_ = nullptr;
	Shader* shader_ad_distortion_ = nullptr;
	Shader* shader_ad_lit_ = nullptr;
	Shader* currentShader = nullptr;

	EffekseerRenderer::StandardRenderer<RendererImplemented, Shader>* m_standardRenderer;

	::Effekseer::Matrix44 m_proj;
	::Effekseer::Matrix44 m_camera;
	::Effekseer::Matrix44 m_cameraProj;

	::Effekseer::Vector3D m_cameraPosition;
	::Effekseer::Vector3D m_cameraFrontDirection;

	// 座標系
	::Effekseer::CoordinateSystem m_coordinateSystem;

	::EffekseerRenderer::RenderStateBase* m_renderState;

	std::set<DeviceObject*> m_deviceObjects;

	// ステート
	DWORD m_state_FVF;

	DWORD m_state_D3DRS_ALPHABLENDENABLE;
	DWORD m_state_D3DRS_BLENDOP;
	DWORD m_state_D3DRS_DESTBLEND;
	DWORD m_state_D3DRS_SRCBLEND;
	DWORD m_state_D3DRS_ALPHAREF;

	DWORD m_state_D3DRS_DESTBLENDALPHA;
	DWORD m_state_D3DRS_SRCBLENDALPHA;
	DWORD m_state_D3DRS_SEPARATEALPHABLENDENABLE;
	DWORD m_state_D3DRS_BLENDOPALPHA;

	DWORD m_state_D3DRS_ZENABLE;
	DWORD m_state_D3DRS_ZWRITEENABLE;
	DWORD m_state_D3DRS_ALPHATESTENABLE;
	DWORD m_state_D3DRS_CULLMODE;

	DWORD m_state_D3DRS_COLORVERTEX;
	DWORD m_state_D3DRS_LIGHTING;
	DWORD m_state_D3DRS_SHADEMODE;

	std::array<DWORD, Effekseer::TextureSlotMax> m_state_D3DSAMP_MAGFILTER;
	std::array<DWORD, Effekseer::TextureSlotMax> m_state_D3DSAMP_MINFILTER;
	std::array<DWORD, Effekseer::TextureSlotMax> m_state_D3DSAMP_MIPFILTER;
	std::array<DWORD, Effekseer::TextureSlotMax> m_state_D3DSAMP_ADDRESSU;
	std::array<DWORD, Effekseer::TextureSlotMax> m_state_D3DSAMP_ADDRESSV;

	IDirect3DVertexShader9* m_state_vertexShader;
	IDirect3DPixelShader9* m_state_pixelShader;
	IDirect3DVertexDeclaration9* m_state_vertexDeclaration;

	std::array<IDirect3DVertexBuffer9*, 2> m_state_streamData;
	std::array<UINT, 2> m_state_OffsetInBytes;
	std::array<UINT, 2> m_state_pStride;

	IDirect3DIndexBuffer9* m_state_IndexData;

	std::vector<float> m_state_VertexShaderConstantF;
	std::vector<float> m_state_PixelShaderConstantF;

	std::array<IDirect3DBaseTexture9*, 2> m_state_pTexture;

	bool m_isChangedDevice;

	bool m_restorationOfStates;

	Backend::GraphicsDeviceRef graphicsDevice_ = nullptr;
	Effekseer::Backend::VertexBufferRef instancedVertexBuffer_;

	EffekseerRenderer::DistortingCallback* m_distortingCallback;

	::Effekseer::Backend::TextureRef m_backgroundDX9;

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

	bool Initialize(Backend::GraphicsDeviceRef graphicsDevice);

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
		@brief	頂点バッファ取得
	*/
	VertexBuffer* GetVertexBuffer();

	/**
		@brief	インデックスバッファ取得
	*/
	IndexBuffer* GetIndexBuffer();

	/**
		@brief	インデックスバッファ取得
	*/
	IndexBuffer* GetIndexBufferForWireframe();

	/**
		@brief	最大描画スプライト数
	*/
	int32_t GetSquareMaxCount() const;

	::EffekseerRenderer::RenderStateBase* GetRenderState();

	/**
		@brief	スプライトレンダラーを生成する。
	*/
	::Effekseer::SpriteRendererRef CreateSpriteRenderer();

	/**
		@brief	リボンレンダラーを生成する。
	*/
	::Effekseer::RibbonRendererRef CreateRibbonRenderer();

	/**
		@brief	リングレンダラーを生成する。
	*/
	::Effekseer::RingRendererRef CreateRingRenderer();

	/**
		@brief	モデルレンダラーを生成する。
	*/
	::Effekseer::ModelRendererRef CreateModelRenderer();

	/**
		@brief	軌跡レンダラーを生成する。
	*/
	::Effekseer::TrackRendererRef CreateTrackRenderer();

	/**
		@brief	テクスチャ読込クラスを生成する。
	*/
	::Effekseer::TextureLoaderRef CreateTextureLoader(::Effekseer::FileInterface* fileInterface = nullptr);

	/**
		@brief	モデル読込クラスを生成する。
	*/
	::Effekseer::ModelLoaderRef CreateModelLoader(::Effekseer::FileInterface* fileInterface = nullptr);

	::Effekseer::MaterialLoaderRef CreateMaterialLoader(::Effekseer::FileInterface* fileInterface = nullptr) override;

	/**
	@brief	背景を設定する。
	*/
	void SetBackground(IDirect3DTexture9* background) override;

	EffekseerRenderer::DistortingCallback* GetDistortingCallback() override;

	void SetDistortingCallback(EffekseerRenderer::DistortingCallback* callback) override;

	EffekseerRenderer::StandardRenderer<RendererImplemented, Shader>* GetStandardRenderer()
	{
		return m_standardRenderer;
	}

	void SetVertexBuffer(VertexBuffer* vertexBuffer, int32_t size);
	void SetVertexBuffer(IDirect3DVertexBuffer9* vertexBuffer, int32_t size);
	void SetIndexBuffer(IndexBuffer* indexBuffer);
	void SetIndexBuffer(IDirect3DIndexBuffer9* indexBuffer);

	void SetVertexBuffer(const Effekseer::Backend::VertexBufferRef& vertexBuffer, int32_t size);
	void SetIndexBuffer(const Effekseer::Backend::IndexBufferRef& indexBuffer);

	void SetLayout(Shader* shader);
	void DrawSprites(int32_t spriteCount, int32_t vertexOffset);
	void DrawPolygon(int32_t vertexCount, int32_t indexCount);
	void DrawPolygonInstanced(int32_t vertexCount, int32_t indexCount, int32_t instanceCount);

	Shader* GetShader(::EffekseerRenderer::RendererShaderType type) const;
	void BeginShader(Shader* shader);
	void EndShader(Shader* shader);

	void SetVertexBufferToShader(const void* data, int32_t size, int32_t dstOffset);

	void SetPixelBufferToShader(const void* data, int32_t size, int32_t dstOffset);

	void SetTextures(Shader* shader, Effekseer::Backend::TextureRef* textures, int32_t count);

	void ChangeDevice(LPDIRECT3DDEVICE9 device);

	void ResetRenderState();

	Effekseer::Backend::GraphicsDeviceRef GetGraphicsDevice() const override;

	LPDIRECT3DDEVICE9 GetDevice() override;

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

private:
	void GenerateIndexData();
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRendererDX9
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_DX9_RENDERER_IMPLEMENTED_H__