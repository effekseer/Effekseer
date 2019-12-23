
#ifndef __EFFEKSEERRENDERER_LLGI_RENDERER_IMPLEMENTED_H__
#define __EFFEKSEERRENDERER_LLGI_RENDERER_IMPLEMENTED_H__

#include "../EffekseerRendererCommon/EffekseerRenderer.RenderStateBase.h"
#include "../EffekseerRendererCommon/EffekseerRenderer.StandardRenderer.h"
#include "../EffekseerRendererCommon/EffekseerRenderer.CommonUtils.h"
#include "EffekseerRendererLLGI.Base.h"
#include "EffekseerRendererLLGI.Renderer.h"
#include <LLGI.CommandList.h>
#include <LLGI.Graphics.h>
#include <LLGI.PipelineState.h>
#include <LLGI.Texture.h>

#if (defined(_M_IX86_FP) && _M_IX86_FP >= 2) || defined(__SSE__)
#define EFK_SSE2
#include <emmintrin.h>
#elif defined(__ARM_NEON__)
#define EFK_NEON
#include <arm_neon.h>
#endif

namespace EffekseerRendererLLGI
{

using Vertex = EffekseerRenderer::SimpleVertex;
using VertexDistortion = EffekseerRenderer::VertexDistortion;

class PiplineStateKey
{
public:
	Shader* shader = nullptr;
	EffekseerRenderer::RenderStateBase::State state;
	LLGI::TopologyType topologyType;
	bool operator<(const PiplineStateKey& v) const;
};

/**
	@brief	描画クラス
	@note
	ツール向けの描画機能。
*/
class RendererImplemented : public Renderer, public ::Effekseer::ReferenceObject
{
	friend class DeviceObject;

private:
	std::map<PiplineStateKey, LLGI::PipelineState*> piplineStates_;
	LLGI::VertexBuffer* currentVertexBuffer_ = nullptr;
	int32_t currentVertexBufferStride_ = 0;
	LLGI::TopologyType currentTopologyType_ = LLGI::TopologyType::Triangle;

	// TODO
	/**
		Create constants and copy
		Shader
	*/

	LLGI::Graphics* graphics_;
	LLGI::RenderPassPipelineState* renderPassPipelineState_ = nullptr;

	VertexBuffer* m_vertexBuffer;
	IndexBuffer* m_indexBuffer;
	IndexBuffer* m_indexBufferForWireframe = nullptr;
	int32_t m_squareMaxCount;

	Shader* m_shader = nullptr;
	Shader* m_shader_lighting = nullptr;
	Shader* m_shader_distortion = nullptr;
	Shader* currentShader = nullptr;

	bool isReversedDepth_ = false;

	EffekseerRenderer::StandardRenderer<RendererImplemented, Shader, Vertex, VertexDistortion>* m_standardRenderer;

	::Effekseer::Vector3D m_lightDirection;
	::Effekseer::Color m_lightColor;
	::Effekseer::Color m_lightAmbient;

	::Effekseer::Matrix44 m_proj;
	::Effekseer::Matrix44 m_camera;
	::Effekseer::Matrix44 m_cameraProj;

	::Effekseer::Vector3D m_cameraPosition;
	::Effekseer::Vector3D m_cameraFrontDirection;

	// 座標系
	::Effekseer::CoordinateSystem m_coordinateSystem;

	::EffekseerRenderer::RenderStateBase* m_renderState;

	Effekseer::TextureData m_background;

	std::set<DeviceObject*> m_deviceObjects;

	EffekseerRenderer::DistortingCallback* m_distortingCallback;

	Effekseer::RenderMode m_renderMode = Effekseer::RenderMode::Normal;

	CommandList* commandList_ = nullptr;

	LLGI::CommandList* GetCurrentCommandList();

	LLGI::PipelineState* GetOrCreatePiplineState();

public:
	//! shaders (todo implemented)
	FixedShader fixedShader_;

	::Effekseer::CompiledMaterialPlatformType platformType_;

	::Effekseer::MaterialCompiler* materialCompiler_ = nullptr;

	RendererImplemented(int32_t squareMaxCount);

	~RendererImplemented();

	void OnLostDevice() override;;
	void OnResetDevice() override;;

	bool Initialize(LLGI::Graphics* graphics, LLGI::RenderPassPipelineState* renderPassPipelineState, bool isReversedDepth);

	void Destroy() override;

	void SetRestorationOfStatesFlag(bool flag) override;

	bool BeginRendering() override;

	bool EndRendering() override;

	void SetCommandList(EffekseerRenderer::CommandList* commandList) override;

	LLGI::Graphics* GetGraphics() override { return graphics_; }

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
	int32_t GetSquareMaxCount() const override;

	::EffekseerRenderer::RenderStateBase* GetRenderState();

	/**
		@brief	ライトの方向を取得する。
	*/
	const ::Effekseer::Vector3D& GetLightDirection() const override;

	/**
		@brief	ライトの方向を設定する。
	*/
	void SetLightDirection(const ::Effekseer::Vector3D& direction) override;

	/**
		@brief	ライトの色を取得する。
	*/
	const ::Effekseer::Color& GetLightColor() const override;

	/**
		@brief	ライトの色を設定する。
	*/
	void SetLightColor(const ::Effekseer::Color& color) override;

	/**
		@brief	ライトの環境光の色を取得する。
	*/
	const ::Effekseer::Color& GetLightAmbientColor() const override;;

	/**
		@brief	ライトの環境光の色を設定する。
	*/
	void SetLightAmbientColor(const ::Effekseer::Color& color) override;;

	/**
		@brief	投影行列を取得する。
	*/
	const ::Effekseer::Matrix44& GetProjectionMatrix() const override;;

	/**
		@brief	投影行列を設定する。
	*/
	void SetProjectionMatrix(const ::Effekseer::Matrix44& mat) override;

	/**
		@brief	カメラ行列を取得する。
	*/
	const ::Effekseer::Matrix44& GetCameraMatrix() const override;

	/**
		@brief	カメラ行列を設定する。
	*/
	void SetCameraMatrix(const ::Effekseer::Matrix44& mat) override;

	::Effekseer::Vector3D GetCameraFrontDirection() const override;

	::Effekseer::Vector3D GetCameraPosition() const override;

	void SetCameraParameter(const ::Effekseer::Vector3D& front, const ::Effekseer::Vector3D& position) override;

	/**
		@brief	カメラプロジェクション行列を取得する。
	*/
	::Effekseer::Matrix44& GetCameraProjectionMatrix() override;

	/**
		@brief	スプライトレンダラーを生成する。
	*/
	::Effekseer::SpriteRenderer* CreateSpriteRenderer() override;

	/**
		@brief	リボンレンダラーを生成する。
	*/
	::Effekseer::RibbonRenderer* CreateRibbonRenderer() override;

	/**
		@brief	リングレンダラーを生成する。
	*/
	::Effekseer::RingRenderer* CreateRingRenderer() override;

	/**
		@brief	モデルレンダラーを生成する。
	*/
	::Effekseer::ModelRenderer* CreateModelRenderer() override;

	/**
		@brief	軌跡レンダラーを生成する。
	*/
	::Effekseer::TrackRenderer* CreateTrackRenderer() override;

	/**
		@brief	テクスチャ読込クラスを生成する。
	*/
	::Effekseer::TextureLoader* CreateTextureLoader(::Effekseer::FileInterface* fileInterface = NULL) override;

	/**
		@brief	モデル読込クラスを生成する。
	*/
	::Effekseer::ModelLoader* CreateModelLoader(::Effekseer::FileInterface* fileInterface = NULL) override;

	::Effekseer::MaterialLoader* CreateMaterialLoader(::Effekseer::FileInterface* fileInterface = nullptr) override;

	/**
	@brief	背景を取得する。
	*/
	Effekseer::TextureData* GetBackground() override;

	void SetBackground(LLGI::Texture* background) override;

	void SetBackgroundTexture(Effekseer::TextureData* textuerData) override;

	EffekseerRenderer::DistortingCallback* GetDistortingCallback() override;

	void SetDistortingCallback(EffekseerRenderer::DistortingCallback* callback) override;

	EffekseerRenderer::StandardRenderer<RendererImplemented, Shader, Vertex, VertexDistortion>* GetStandardRenderer()
	{
		return m_standardRenderer;
	}

	void SetVertexBuffer(VertexBuffer* vertexBuffer, int32_t stride);
	void SetVertexBuffer(LLGI::VertexBuffer* vertexBuffer, int32_t stride);
	void SetIndexBuffer(IndexBuffer* indexBuffer);
	void SetIndexBuffer(LLGI::IndexBuffer* indexBuffer);

	void SetLayout(Shader* shader);
	void DrawSprites(int32_t spriteCount, int32_t vertexOffset);
	void DrawPolygon(int32_t vertexCount, int32_t indexCount);

	Shader* GetShader(bool useTexture, ::Effekseer::RendererMaterialType materialType) const;
	void BeginShader(Shader* shader);
	void EndShader(Shader* shader);

	void SetVertexBufferToShader(const void* data, int32_t size, int32_t dstOffset);

	void SetPixelBufferToShader(const void* data, int32_t size, int32_t dstOffset);

	void SetTextures(Shader* shader, Effekseer::TextureData** textures, int32_t count);

	void ResetRenderState() override;

	Effekseer::TextureData* CreateProxyTexture(EffekseerRenderer::ProxyTextureType type) override;

	void DeleteProxyTexture(Effekseer::TextureData* data) override;

	virtual int GetRef() override { return ::Effekseer::ReferenceObject::GetRef(); }
	virtual int AddRef() override { return ::Effekseer::ReferenceObject::AddRef(); }
	virtual int Release() override { return ::Effekseer::ReferenceObject::Release(); }
};

} // namespace EffekseerRendererLLGI

#endif // __EFFEKSEERRENDERER_LLGI_RENDERER_IMPLEMENTED_H__
