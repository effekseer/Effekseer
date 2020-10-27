
#ifndef __EFFEKSEERRENDERER_GL_RENDERER_IMPLEMENTED_H__
#define __EFFEKSEERRENDERER_GL_RENDERER_IMPLEMENTED_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "../../EffekseerRendererCommon/EffekseerRenderer.RenderStateBase.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.StandardRenderer.h"
#include "EffekseerRendererGL.Base.h"
#include "EffekseerRendererGL.DeviceObjectCollection.h"
#include "EffekseerRendererGL.Renderer.h"

namespace EffekseerRendererGL
{

using Vertex = EffekseerRenderer::SimpleVertex;
using VertexDistortion = EffekseerRenderer::VertexDistortion;

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
class RendererImplemented
	: public Renderer,
	  public ::Effekseer::ReferenceObject
{
	friend class DeviceObject;

private:
	DeviceObjectCollection* deviceObjectCollection_ = nullptr;

	VertexBuffer* m_vertexBuffer;
	IndexBuffer* m_indexBuffer = nullptr;
	IndexBuffer* m_indexBufferForWireframe = nullptr;
	int32_t m_squareMaxCount;

	Shader* m_shader = nullptr;
	Shader* m_shader_distortion = nullptr;
	Shader* m_shader_lighting = nullptr;
	Shader* currentShader = nullptr;

	EffekseerRenderer::StandardRenderer<RendererImplemented, Shader, Vertex, VertexDistortion>* m_standardRenderer;

	VertexArray* m_vao = nullptr;
	VertexArray* m_vao_distortion = nullptr;
	VertexArray* m_vao_lighting = nullptr;
	VertexArray* m_vao_wire_frame = nullptr;

	//! default vao (alsmot for material)
	GLuint defaultVertexArray_ = 0;

	::EffekseerRenderer::RenderStateBase* m_renderState;

	Effekseer::TextureData m_background;

	OpenGLDeviceType m_deviceType;

	// for restoring states
	RenderStateSet m_originalState;

	bool m_restorationOfStates;

	EffekseerRenderer::DistortingCallback* m_distortingCallback;

	// textures which are specified currently
	std::vector<::Effekseer::TextureData> currentTextures_;

	VertexArray* m_currentVertexArray;

	int32_t indexBufferStride_ = 2;

	int32_t indexBufferCurrentStride_ = 0;

	//! because gleDrawElements has only index offset
	int32_t GetIndexSpriteCount() const;

public:
	RendererImplemented(int32_t squareMaxCount, OpenGLDeviceType deviceType, DeviceObjectCollection* deviceObjectCollection);

	~RendererImplemented();

	void OnLostDevice() override;
	void OnResetDevice() override;

	bool Initialize();

	void Destroy() override;

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
	Effekseer::TextureData* GetBackground() override
	{
		if (m_background.UserID == 0)
			return nullptr;
		return &m_background;
	}

	void SetBackground(GLuint background, bool hasMipmap) override;

	void SetBackgroundTexture(::Effekseer::TextureData* textureData) override;

	EffekseerRenderer::DistortingCallback* GetDistortingCallback() override;

	void SetDistortingCallback(EffekseerRenderer::DistortingCallback* callback) override;

	EffekseerRenderer::StandardRenderer<RendererImplemented, Shader, Vertex, VertexDistortion>*
	GetStandardRenderer()
	{
		return m_standardRenderer;
	}

	void SetVertexBuffer(VertexBuffer* vertexBuffer, int32_t size);
	void SetVertexBuffer(GLuint vertexBuffer, int32_t size);
	void SetIndexBuffer(IndexBuffer* indexBuffer);
	void SetIndexBuffer(GLuint indexBuffer);
	void SetVertexArray(VertexArray* vertexArray);

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

	const std::vector<::Effekseer::TextureData>& GetCurrentTextures() const
	{
		return currentTextures_;
	}

	OpenGLDeviceType GetDeviceType() const override
	{
		return m_deviceType;
	}

	bool IsVertexArrayObjectSupported() const override;

	DeviceObjectCollection* GetDeviceObjectCollection() const
	{
		return deviceObjectCollection_;
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

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRendererGL
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_GL_RENDERER_IMPLEMENTED_H__
