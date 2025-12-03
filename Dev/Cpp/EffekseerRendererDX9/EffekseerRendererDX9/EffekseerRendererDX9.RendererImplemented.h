
#ifndef __EFFEKSEERRENDERER_DX9_RENDERER_IMPLEMENTED_H__
#define __EFFEKSEERRENDERER_DX9_RENDERER_IMPLEMENTED_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <EffekseerRendererCommon/EffekseerRenderer.RenderStateBase.h>
#include <EffekseerRendererCommon/EffekseerRenderer.StandardRenderer.h>
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
	Effekseer::Backend::IndexBufferRef currentndexBuffer_;
	Effekseer::Backend::IndexBufferRef indexBuffer_;
	Effekseer::Backend::IndexBufferRef indexBufferForWireframe_;

	int32_t squareMaxCount_;
	Shader* currentShader = nullptr;

	EffekseerRenderer::StandardRenderer<RendererImplemented, Shader>* standardRenderer_;

	::Effekseer::Matrix44 proj_;
	::Effekseer::Matrix44 camera_;
	::Effekseer::Matrix44 cameraProj_;

	::Effekseer::Vector3D cameraPosition_;
	::Effekseer::Vector3D cameraFrontDirection_;

	// 座標系
	::Effekseer::CoordinateSystem coordinateSystem_;

	::EffekseerRenderer::RenderStateBase* renderState_;

	// ステート
	DWORD stateFvf_;

	DWORD stateAlphaBlendEnable_;
	DWORD stateBlendOp_;
	DWORD stateDestBlend_;
	DWORD stateSrcBlend_;
	DWORD stateAlphaRef_;

	DWORD stateDestBlendAlpha_;
	DWORD stateSrcBlendAlpha_;
	DWORD stateSeparateAlphaBlendEnable_;
	DWORD stateBlendOpAlpha_;

	DWORD stateZEnable_;
	DWORD stateZWriteEnable_;
	DWORD stateAlphaTestEnable_;
	DWORD stateCullMode_;

	DWORD stateColorVertex_;
	DWORD stateLighting_;
	DWORD stateShadeMode_;

	std::array<DWORD, Effekseer::TextureSlotMax> stateSamplerMagFilter_;
	std::array<DWORD, Effekseer::TextureSlotMax> stateSamplerMinFilter_;
	std::array<DWORD, Effekseer::TextureSlotMax> stateSamplerMipFilter_;
	std::array<DWORD, Effekseer::TextureSlotMax> stateSamplerAddressU_;
	std::array<DWORD, Effekseer::TextureSlotMax> stateSamplerAddressV_;

	IDirect3DVertexShader9* stateVertexShader_;
	IDirect3DPixelShader9* statePixelShader_;
	IDirect3DVertexDeclaration9* stateVertexDeclaration_;

	std::array<IDirect3DVertexBuffer9*, 2> stateStreamData_;
	std::array<UINT, 2> stateStreamFreq_;
	std::array<UINT, 2> stateOffsetInBytes_;
	std::array<UINT, 2> stateStride_;

	IDirect3DIndexBuffer9* stateIndexData_;

	std::vector<float> stateVertexShaderConstantF_;
	std::vector<float> statePixelShaderConstantF_;

	std::array<IDirect3DBaseTexture9*, 2> stateTextures_;

	bool isChangedDevice_;

	bool restorationOfStates_;

	Backend::GraphicsDeviceRef graphicsDevice_ = nullptr;
	Effekseer::Backend::VertexBufferRef instancedVertexBuffer_;

	EffekseerRenderer::DistortingCallback* distortingCallback_;

	::Effekseer::Backend::TextureRef backgroundDX9_;

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

	Effekseer::Backend::IndexBufferRef GetIndexBuffer();

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
	::Effekseer::TextureLoaderRef CreateTextureLoader(::Effekseer::FileInterfaceRef fileInterface = nullptr);

	/**
		@brief	モデル読込クラスを生成する。
	*/
	::Effekseer::ModelLoaderRef CreateModelLoader(::Effekseer::FileInterfaceRef fileInterface = nullptr);

	::Effekseer::MaterialLoaderRef CreateMaterialLoader(::Effekseer::FileInterfaceRef fileInterface = nullptr) override;

	/**
	@brief	背景を設定する。
	*/
	void SetBackground(IDirect3DTexture9* background) override;

	EffekseerRenderer::DistortingCallback* GetDistortingCallback() override;

	void SetDistortingCallback(EffekseerRenderer::DistortingCallback* callback) override;

	EffekseerRenderer::StandardRenderer<RendererImplemented, Shader>* GetStandardRenderer()
	{
		return standardRenderer_;
	}

	void SetVertexBuffer(const Effekseer::Backend::VertexBufferRef& vertexBuffer, int32_t size);
	void SetIndexBuffer(const Effekseer::Backend::IndexBufferRef& indexBuffer);

	void SetLayout(Shader* shader);
	void DrawSprites(int32_t spriteCount, int32_t vertexOffset);
	void DrawPolygon(int32_t vertexCount, int32_t indexCount);
	void DrawPolygonInstanced(int32_t vertexCount, int32_t indexCount, int32_t instanceCount);

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
	// void GenerateIndexData();
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRendererDX9
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_DX9_RENDERER_IMPLEMENTED_H__