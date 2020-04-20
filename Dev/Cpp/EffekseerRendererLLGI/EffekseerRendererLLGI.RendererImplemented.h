
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
    LLGI::RenderPassPipelineState* renderPassPipelineState = nullptr;
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

protected:
	std::map<PiplineStateKey, LLGI::PipelineState*> piplineStates_;
	LLGI::VertexBuffer* currentVertexBuffer_ = nullptr;
	int32_t currentVertexBufferStride_ = 0;
	LLGI::TopologyType currentTopologyType_ = LLGI::TopologyType::Triangle;

	// TODO
	/**
		Create constants and copy
		Shader
	*/

	GraphicsDevice* graphicsDevice_ = nullptr;
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

	::Effekseer::CoordinateSystem m_coordinateSystem;

	::EffekseerRenderer::RenderStateBase* m_renderState;

	Effekseer::TextureData m_background;

	std::set<DeviceObject*> m_deviceObjects;

	EffekseerRenderer::DistortingCallback* m_distortingCallback;

	Effekseer::RenderMode m_renderMode = Effekseer::RenderMode::Normal;

	CommandList* commandList_ = nullptr;

	LLGI::CommandList* GetCurrentCommandList();

	LLGI::PipelineState* GetOrCreatePiplineState();

    virtual void GenerateVertexBuffer();
    
	virtual void GenerateIndexBuffer();

public:
	//! shaders (todo implemented)
	FixedShader fixedShader_;

	::Effekseer::CompiledMaterialPlatformType platformType_;

	::Effekseer::MaterialCompiler* materialCompiler_ = nullptr;

	RendererImplemented(int32_t squareMaxCount);

	~RendererImplemented();

	void OnLostDevice() override;;
	void OnResetDevice() override;;

	bool Initialize(GraphicsDevice* graphicsDevice, LLGI::RenderPassPipelineState* renderPassPipelineState, bool isReversedDepth);

	bool Initialize(LLGI::Graphics* graphics, LLGI::RenderPassPipelineState* renderPassPipelineState, bool isReversedDepth);

	void Destroy() override;

	void SetRestorationOfStatesFlag(bool flag) override;

    void SetRenderPassPipelineState(LLGI::RenderPassPipelineState* renderPassPipelineState);

	bool BeginRendering() override;

	bool EndRendering() override;

	void SetCommandList(EffekseerRenderer::CommandList* commandList) override;

	GraphicsDevice* GetGraphicsDevice() const { return graphicsDevice_; }

	LLGI::Graphics* GetGraphics() const override { return graphicsDevice_->GetGraphics(); }

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
