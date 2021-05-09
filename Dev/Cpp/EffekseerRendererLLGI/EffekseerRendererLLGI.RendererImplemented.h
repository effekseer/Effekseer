
#ifndef __EFFEKSEERRENDERER_LLGI_RENDERER_IMPLEMENTED_H__
#define __EFFEKSEERRENDERER_LLGI_RENDERER_IMPLEMENTED_H__

#include "../EffekseerRendererCommon/EffekseerRenderer.CommonUtils.h"
#include "../EffekseerRendererCommon/EffekseerRenderer.RenderStateBase.h"
#include "../EffekseerRendererCommon/EffekseerRenderer.StandardRenderer.h"
#include "EffekseerRendererLLGI.Base.h"
#include "EffekseerRendererLLGI.Renderer.h"
#include <LLGI.CommandList.h>
#include <LLGI.Graphics.h>
#include <LLGI.PipelineState.h>
#include <LLGI.Texture.h>

namespace EffekseerRendererLLGI
{

using Vertex = EffekseerRenderer::SimpleVertex;
//using VertexDistortion = EffekseerRenderer::VertexDistortion;

class PiplineStateKey
{
public:
	Shader* shader = nullptr;
	EffekseerRenderer::RenderStateBase::State state;
	LLGI::TopologyType topologyType;
	LLGI::RenderPassPipelineState* renderPassPipelineState = nullptr;
	bool operator<(const PiplineStateKey& v) const;
};

LLGI::TextureFormatType ConvertTextureFormat(Effekseer::Backend::TextureFormatType format);

class RendererImplemented : public Renderer, public ::Effekseer::ReferenceObject
{
	friend class DeviceObject;

protected:
	std::map<PiplineStateKey, LLGI::PipelineState*> piplineStates_;
	LLGI::VertexBuffer* currentVertexBuffer_ = nullptr;
	int32_t currentVertexBufferStride_ = 0;
	LLGI::TopologyType currentTopologyType_ = LLGI::TopologyType::Triangle;

	std::unordered_map<LLGI::RenderPassPipelineStateKey, std::shared_ptr<LLGI::RenderPassPipelineState>, LLGI::RenderPassPipelineStateKey::Hash> renderpassPipelineStates_;

	// TODO
	/**
		Create constants and copy
		Shader
	*/

	Backend::GraphicsDeviceRef graphicsDevice_ = nullptr;
	std::shared_ptr<LLGI::RenderPassPipelineState> currentRenderPassPipelineState_ = nullptr;

	VertexBuffer* m_vertexBuffer;
	IndexBuffer* m_indexBuffer;
	IndexBuffer* m_indexBufferForWireframe = nullptr;
	int32_t m_squareMaxCount;

	Shader* shader_unlit_ = nullptr;
	Shader* shader_lit_ = nullptr;
	Shader* shader_distortion_ = nullptr;

	Shader* shader_ad_unlit_ = nullptr;
	Shader* shader_ad_lit_ = nullptr;
	Shader* shader_ad_distortion_ = nullptr;

	Shader* currentShader = nullptr;

	bool isReversedDepth_ = false;

	EffekseerRenderer::StandardRenderer<RendererImplemented, Shader>* m_standardRenderer;

	::Effekseer::CoordinateSystem m_coordinateSystem;

	::EffekseerRenderer::RenderStateBase* m_renderState;

	std::set<DeviceObject*> m_deviceObjects;

	EffekseerRenderer::DistortingCallback* m_distortingCallback;

	::Effekseer::Backend::TextureRef m_backgroundLLGI;

	Effekseer::RenderMode m_renderMode = Effekseer::RenderMode::Normal;

	Effekseer::RefPtr<EffekseerRenderer::CommandList> commandList_ = nullptr;

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

	void OnLostDevice() override;

	void OnResetDevice() override;

	bool Initialize(Backend::GraphicsDeviceRef graphicsDevice, LLGI::RenderPassPipelineStateKey key, bool isReversedDepth);

	bool Initialize(LLGI::Graphics* graphics, LLGI::RenderPassPipelineStateKey key, bool isReversedDepth);

	void SetRestorationOfStatesFlag(bool flag) override;

	void ChangeRenderPassPipelineState(LLGI::RenderPassPipelineStateKey key);

	bool BeginRendering() override;

	bool EndRendering() override;

	void SetCommandList(Effekseer::RefPtr<EffekseerRenderer::CommandList> commandList) override;

	Effekseer::Backend::GraphicsDeviceRef GetGraphicsDevice() const override
	{
		return graphicsDevice_;
	}

	Backend::GraphicsDeviceRef& GetGraphicsDeviceInternal()
	{
		return graphicsDevice_;
	}

	LLGI::Graphics* GetGraphics() const override
	{
		return graphicsDevice_->GetGraphics();
	}

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
	::Effekseer::SpriteRendererRef CreateSpriteRenderer() override;

	/**
		@brief	リボンレンダラーを生成する。
	*/
	::Effekseer::RibbonRendererRef CreateRibbonRenderer() override;

	/**
		@brief	リングレンダラーを生成する。
	*/
	::Effekseer::RingRendererRef CreateRingRenderer() override;

	/**
		@brief	モデルレンダラーを生成する。
	*/
	::Effekseer::ModelRendererRef CreateModelRenderer() override;

	/**
		@brief	軌跡レンダラーを生成する。
	*/
	::Effekseer::TrackRendererRef CreateTrackRenderer() override;

	/**
		@brief	テクスチャ読込クラスを生成する。
	*/
	::Effekseer::TextureLoaderRef CreateTextureLoader(::Effekseer::FileInterface* fileInterface = nullptr) override;

	/**
		@brief	モデル読込クラスを生成する。
	*/
	::Effekseer::ModelLoaderRef CreateModelLoader(::Effekseer::FileInterface* fileInterface = nullptr) override;

	::Effekseer::MaterialLoaderRef CreateMaterialLoader(::Effekseer::FileInterface* fileInterface = nullptr) override;

	void SetBackgroundInternal(LLGI::Texture* background);

	EffekseerRenderer::DistortingCallback* GetDistortingCallback() override;

	void SetDistortingCallback(EffekseerRenderer::DistortingCallback* callback) override;

	EffekseerRenderer::StandardRenderer<RendererImplemented, Shader>* GetStandardRenderer()
	{
		return m_standardRenderer;
	}

	void SetVertexBuffer(VertexBuffer* vertexBuffer, int32_t stride);
	void SetVertexBuffer(LLGI::VertexBuffer* vertexBuffer, int32_t stride);
	void SetIndexBuffer(IndexBuffer* indexBuffer);
	void SetIndexBuffer(LLGI::IndexBuffer* indexBuffer);

	void SetVertexBuffer(const Effekseer::Backend::VertexBufferRef& vertexBuffer, int32_t stride);
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

	void ResetRenderState() override;

	virtual int GetRef() override
	{
		return ::Effekseer::ReferenceObject::GetRef();
	}
	virtual int AddRef() override
	{
		return ::Effekseer::ReferenceObject::AddRef();
	}
	virtual int Release() override
	{
		return ::Effekseer::ReferenceObject::Release();
	}
};

} // namespace EffekseerRendererLLGI

#endif // __EFFEKSEERRENDERER_LLGI_RENDERER_IMPLEMENTED_H__
