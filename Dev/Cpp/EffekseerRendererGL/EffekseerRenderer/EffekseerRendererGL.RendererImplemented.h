
#ifndef __EFFEKSEERRENDERER_GL_RENDERER_IMPLEMENTED_H__
#define __EFFEKSEERRENDERER_GL_RENDERER_IMPLEMENTED_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "../../EffekseerRendererCommon/EffekseerRenderer.RenderStateBase.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.StandardRenderer.h"
#include "EffekseerRendererGL.Base.h"
#include "EffekseerRendererGL.Renderer.h"
#include "GraphicsDevice.h"

namespace EffekseerRendererGL
{

using Vertex = EffekseerRenderer::SimpleVertex;
//using VertexDistortion = EffekseerRenderer::VertexDistortion;

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
	GLint blendEquation;
	GLint vao;
	GLint arrayBufferBinding;
	GLint elementArrayBufferBinding;
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

	VertexBuffer* m_vertexBuffer;
	IndexBuffer* m_indexBuffer = nullptr;
	IndexBuffer* m_indexBufferForWireframe = nullptr;
	int32_t m_squareMaxCount;

	Shader* shader_unlit_ = nullptr;
	Shader* shader_distortion_ = nullptr;
	Shader* shader_lit_ = nullptr;
	Shader* shader_ad_unlit_ = nullptr;
	Shader* shader_ad_lit_ = nullptr;
	Shader* shader_ad_distortion_ = nullptr;

	Shader* currentShader = nullptr;

	EffekseerRenderer::StandardRenderer<RendererImplemented, Shader>* m_standardRenderer;

	VertexArray* vao_unlit_ = nullptr;
	VertexArray* vao_distortion_ = nullptr;
	VertexArray* vao_lit_ = nullptr;
	VertexArray* vao_ad_unlit_ = nullptr;
	VertexArray* vao_ad_lit_ = nullptr;
	VertexArray* vao_ad_distortion_ = nullptr;

	VertexArray* vao_unlit_wire_ = nullptr;
	VertexArray* vao_distortion_wire_ = nullptr;
	VertexArray* vao_lit_wire_ = nullptr;
	VertexArray* vao_ad_unlit_wire_ = nullptr;
	VertexArray* vao_ad_distortion_wire_ = nullptr;
	VertexArray* vao_ad_lit_wire_ = nullptr;

	//! default vao (alsmot for material)
	GLuint defaultVertexArray_ = 0;

	::EffekseerRenderer::RenderStateBase* m_renderState;

	OpenGLDeviceType m_deviceType;

	// for restoring states
	RenderStateSet m_originalState;

	bool m_restorationOfStates;

	EffekseerRenderer::DistortingCallback* m_distortingCallback;

	::Effekseer::Backend::TextureRef m_backgroundGL;

	// textures which are specified currently
	std::vector<::Effekseer::Backend::TextureRef> currentTextures_;

	VertexArray* m_currentVertexArray;

	int32_t indexBufferStride_ = 2;

	int32_t indexBufferCurrentStride_ = 0;

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
		@brief	テクスチャ読込クラスを生成する。
	*/
	::Effekseer::TextureLoaderRef CreateTextureLoader(::Effekseer::FileInterface* fileInterface = nullptr) override;

	/**
		@brief	モデル読込クラスを生成する。
	*/
	::Effekseer::ModelLoaderRef CreateModelLoader(::Effekseer::FileInterface* fileInterface = nullptr) override;

	::Effekseer::MaterialLoaderRef CreateMaterialLoader(::Effekseer::FileInterface* fileInterface = nullptr) override;

	void SetBackground(GLuint background, bool hasMipmap) override;

	EffekseerRenderer::DistortingCallback* GetDistortingCallback() override;

	void SetDistortingCallback(EffekseerRenderer::DistortingCallback* callback) override;

	EffekseerRenderer::StandardRenderer<RendererImplemented, Shader>* GetStandardRenderer()
	{
		return m_standardRenderer;
	}

	void SetVertexBuffer(VertexBuffer* vertexBuffer, int32_t size);
	void SetVertexBuffer(GLuint vertexBuffer, int32_t size);
	void SetIndexBuffer(IndexBuffer* indexBuffer);
	void SetIndexBuffer(GLuint indexBuffer);

	void SetVertexBuffer(const Effekseer::Backend::VertexBufferRef& vertexBuffer, int32_t size);
	void SetIndexBuffer(const Effekseer::Backend::IndexBufferRef& indexBuffer);

	void SetVertexArray(VertexArray* vertexArray);

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

	const std::vector<::Effekseer::Backend::TextureRef>& GetCurrentTextures() const
	{
		return currentTextures_;
	}

	OpenGLDeviceType GetDeviceType() const override
	{
		return m_deviceType;
	}

	bool IsVertexArrayObjectSupported() const override;

	Backend::GraphicsDeviceRef& GetIntetnalGraphicsDevice()
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
	void GenerateIndexData();

	template <typename T>
	void GenerateIndexDataStride();
};

void AssignPixelConstantBuffer(Shader* shader);

void AssignDistortionPixelConstantBuffer(Shader* shader);

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRendererGL
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_GL_RENDERER_IMPLEMENTED_H__
