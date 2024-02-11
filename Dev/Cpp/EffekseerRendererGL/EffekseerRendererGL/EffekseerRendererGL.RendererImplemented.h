
#ifndef __EFFEKSEERRENDERER_GL_RENDERER_IMPLEMENTED_H__
#define __EFFEKSEERRENDERER_GL_RENDERER_IMPLEMENTED_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererGL.Base.h"
#include "EffekseerRendererGL.Renderer.h"
#include "GraphicsDevice.h"
#include <EffekseerRendererCommon/EffekseerRenderer.RenderStateBase.h>
#include <EffekseerRendererCommon/EffekseerRenderer.StandardRenderer.h>

namespace EffekseerRendererGL
{

using Vertex = EffekseerRenderer::SimpleVertex;
// using VertexDistortion = EffekseerRenderer::VertexDistortion;

struct RenderStateSet
{
	GLboolean blend;
	GLboolean cullFace;
	GLboolean depthTest;
	GLboolean depthWrite;
	GLboolean texture;
	GLint depthFunc;
	GLint cullFaceMode;
	GLint blendSrc;
	GLint blendDst;
	GLint blendSrcAlpha;
	GLint blendDstAlpha;
	GLint blendEquation;
	GLint vao;
	GLint arrayBufferBinding;
	GLint elementArrayBufferBinding;
	GLint program;
	std::array<GLint, ::Effekseer::TextureSlotMax> boundTextures;
};

/**
	@brief	描画クラス
	@note
	ツール向けの描画機能。
*/
class RendererImplemented;
using RendererImplementedRef = ::Effekseer::RefPtr<RendererImplemented>;

class RendererImplemented : public Renderer, public ::Effekseer::ReferenceObject
{
	friend class DeviceObject;

private:
	Backend::GraphicsDeviceRef graphicsDevice_ = nullptr;
	Effekseer::Backend::IndexBufferStrideType indexBufferStride_ = Effekseer::Backend::IndexBufferStrideType::Stride2;
	Effekseer::Backend::IndexBufferRef currentndexBuffer_;
	Effekseer::Backend::IndexBufferRef indexBuffer_;
	Effekseer::Backend::IndexBufferRef indexBufferForWireframe_;

	int32_t m_squareMaxCount;

	Shader* currentShader = nullptr;

	EffekseerRenderer::StandardRenderer<RendererImplemented, Shader>* m_standardRenderer;

	::EffekseerRenderer::RenderStateBase* m_renderState;

	OpenGLDeviceType m_deviceType;

	// for restoring states
	RenderStateSet m_originalState;

	bool m_restorationOfStates;

	EffekseerRenderer::DistortingCallback* m_distortingCallback;

	::Effekseer::Backend::TextureRef m_backgroundGL;

	// textures which are specified currently
	std::vector<::Effekseer::Backend::TextureRef> currentTextures_;

	std::unique_ptr<::EffekseerRendererGL::Backend::VertexArrayObject> renderingVAO_;

	//! because gleDrawElements has only index offset
	int32_t GetIndexSpriteCount() const;

public:
	RendererImplemented(int32_t squareMaxCount, Backend::GraphicsDeviceRef graphicsDevice);

	~RendererImplemented();

	void OnLostDevice() override;
	void OnResetDevice() override;

	bool Initialize();

	void SetRestorationOfStatesFlag(bool flag) override;

	/**
		@brief	描画開始
	*/
	bool BeginRendering() override;

	/**
		@brief	描画終了
	*/
	bool EndRendering() override;

	/**
		@brief	インデックスバッファ取得
	*/
	// IndexBuffer* GetIndexBuffer();

	Effekseer::Backend::IndexBufferRef GetIndexBuffer();

	/**
		@brief	最大描画スプライト数
	*/
	int32_t GetSquareMaxCount() const override;

	void SetSquareMaxCount(int32_t count) override;

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
	@brief	GPUタイマーを生成する。
	*/
	::Effekseer::GpuTimerRef CreateGpuTimer() override;

	/**
		@brief	テクスチャ読込クラスを生成する。
	*/
	::Effekseer::TextureLoaderRef CreateTextureLoader(::Effekseer::FileInterfaceRef fileInterface = nullptr) override;

	/**
		@brief	モデル読込クラスを生成する。
	*/
	::Effekseer::ModelLoaderRef CreateModelLoader(::Effekseer::FileInterfaceRef fileInterface = nullptr) override;

	::Effekseer::MaterialLoaderRef CreateMaterialLoader(::Effekseer::FileInterfaceRef fileInterface = nullptr) override;

	void SetBackground(GLuint background, bool hasMipmap) override;

	EffekseerRenderer::DistortingCallback* GetDistortingCallback() override;

	void SetDistortingCallback(EffekseerRenderer::DistortingCallback* callback) override;

	EffekseerRenderer::StandardRenderer<RendererImplemented, Shader>* GetStandardRenderer()
	{
		return m_standardRenderer;
	}

	void SetVertexBuffer(GLuint vertexBuffer, int32_t size);

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

	void ResetRenderState() override;

	const std::vector<::Effekseer::Backend::TextureRef>& GetCurrentTextures() const
	{
		return currentTextures_;
	}

	OpenGLDeviceType GetDeviceType() const override
	{
		return m_deviceType;
	}

	bool IsVertexArrayObjectSupported() const override;

	Backend::GraphicsDeviceRef& GetInternalGraphicsDevice()
	{
		return graphicsDevice_;
	}

	Effekseer::Backend::GraphicsDeviceRef GetGraphicsDevice() const override
	{
		return graphicsDevice_;
	}

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

private:
	bool GenerateIndexData();
};

void AddVertexUniformLayout(Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement>& uniformLayout);

void AddPixelUniformLayout(Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement>& uniformLayout);

void AddDistortionPixelUniformLayout(Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement>& uniformLayout);

Effekseer::CustomVector<Effekseer::CustomString<char>> GetTextureLocations(EffekseerRenderer::RendererShaderType type);

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRendererGL
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_GL_RENDERER_IMPLEMENTED_H__
