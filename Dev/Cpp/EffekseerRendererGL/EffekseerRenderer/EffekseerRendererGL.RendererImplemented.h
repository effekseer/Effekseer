
#ifndef	__EFFEKSEERRENDERER_GL_RENDERER_IMPLEMENTED_H__
#define	__EFFEKSEERRENDERER_GL_RENDERER_IMPLEMENTED_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererGL.Base.h"
#include "EffekseerRendererGL.Renderer.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.RenderStateBase.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.StandardRenderer.h"

//----------------------------------------------------------------------------------
// Lib
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace EffekseerRendererGL
{

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct Vertex
{
	::Effekseer::Vector3D	Pos;
	::Effekseer::Color		Col;
	float					UV[2];

	void SetColor( const ::Effekseer::Color& color )
	{
		Col = color;
	}
};

struct VertexDistortion
{
	::Effekseer::Vector3D	Pos;
	::Effekseer::Color		Col;
	float					UV[2];
	::Effekseer::Vector3D	Tangent;
	::Effekseer::Vector3D	Binormal;

	void SetColor(const ::Effekseer::Color& color)
	{
		Col = color;
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct RenderStateSet
{
	GLboolean	blend;
	GLboolean	cullFace;
	GLboolean	depthTest;
	GLboolean	depthWrite;
	GLboolean	texture;
	GLint		depthFunc;
	GLint		cullFaceMode;
	GLint		blendSrc;
	GLint		blendDst;
	GLint		blendEquation;
	GLint		vao;
};

/**
	@brief	描画クラス
	@note
	ツール向けの描画機能。
*/
class RendererImplemented
	: public Renderer
	, public ::Effekseer::ReferenceObject
{
friend class DeviceObject;

private:
	VertexBuffer*		m_vertexBuffer;
	IndexBuffer*		m_indexBuffer = nullptr;
	IndexBuffer*		m_indexBufferForWireframe = nullptr;
	int32_t				m_squareMaxCount;
	
	int32_t				drawcallCount = 0;
	int32_t				drawvertexCount = 0;

	Shader*							m_shader;
	Shader*							m_shader_no_texture;

	Shader*							m_shader_distortion;
	Shader*							m_shader_no_texture_distortion;

	Shader*		currentShader = nullptr;

	EffekseerRenderer::StandardRenderer<RendererImplemented, Shader, Vertex, VertexDistortion>*	m_standardRenderer;

	VertexArray*			m_vao;
	VertexArray*			m_vao_no_texture;

	VertexArray*			m_vao_distortion;
	VertexArray*			m_vao_no_texture_distortion;

	VertexArray*			m_vao_wire_frame;

	::Effekseer::Vector3D	m_lightDirection;
	::Effekseer::Color		m_lightColor;
	::Effekseer::Color		m_lightAmbient;

	::Effekseer::Matrix44	m_proj;
	::Effekseer::Matrix44	m_camera;
	::Effekseer::Matrix44	m_cameraProj;

	::Effekseer::Vector3D	m_cameraPosition;
	::Effekseer::Vector3D	m_cameraFrontDirection;

	::EffekseerRenderer::RenderStateBase*		m_renderState;

	Effekseer::TextureData	m_background;

	std::set<DeviceObject*>	m_deviceObjects;

	OpenGLDeviceType		m_deviceType;

	// ステート保存用
	RenderStateSet m_originalState;

	bool	m_restorationOfStates;

	EffekseerRenderer::DistortingCallback* m_distortingCallback;

	/* 現在設定されているテクスチャ */
	std::vector<GLuint>	m_currentTextures;

	VertexArray*	m_currentVertexArray;

	Effekseer::RenderMode m_renderMode = Effekseer::RenderMode::Normal;

public:
	/**
		@brief	コンストラクタ
	*/
	RendererImplemented(int32_t squareMaxCount, OpenGLDeviceType deviceType);

	/**
		@brief	デストラクタ
	*/
	~RendererImplemented();

	void OnLostDevice() override;
	void OnResetDevice() override;

	/**
		@brief	初期化
	*/
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
		@brief	ライトの方向を取得する。
	*/
	const ::Effekseer::Vector3D& GetLightDirection() const override;

	/**
		@brief	ライトの方向を設定する。
	*/
	void SetLightDirection( const ::Effekseer::Vector3D& direction ) override;

	/**
		@brief	ライトの色を取得する。
	*/
	const ::Effekseer::Color& GetLightColor() const override;

	/**
		@brief	ライトの色を設定する。
	*/
	void SetLightColor( const ::Effekseer::Color& color ) override;

	/**
		@brief	ライトの環境光の色を取得する。
	*/
	const ::Effekseer::Color& GetLightAmbientColor() const override;

	/**
		@brief	ライトの環境光の色を設定する。
	*/
	void SetLightAmbientColor( const ::Effekseer::Color& color ) override;

	/**
		@brief	投影行列を取得する。
	*/
	const ::Effekseer::Matrix44& GetProjectionMatrix() const override;

	/**
		@brief	投影行列を設定する。
	*/
	void SetProjectionMatrix( const ::Effekseer::Matrix44& mat ) override;

	/**
		@brief	カメラ行列を取得する。
	*/
	const ::Effekseer::Matrix44& GetCameraMatrix() const override;

	/**
		@brief	カメラ行列を設定する。
	*/
	void SetCameraMatrix( const ::Effekseer::Matrix44& mat ) override;

	/**
		@brief	カメラプロジェクション行列を取得する。
	*/
	::Effekseer::Matrix44& GetCameraProjectionMatrix() override;

	::Effekseer::Vector3D GetCameraFrontDirection() const override;

	::Effekseer::Vector3D GetCameraPosition() const  override;

	void SetCameraParameter(const ::Effekseer::Vector3D& front, const ::Effekseer::Vector3D& position)  override;

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
	::Effekseer::TextureLoader* CreateTextureLoader( ::Effekseer::FileInterface* fileInterface = NULL )override;
	
	/**
		@brief	モデル読込クラスを生成する。
	*/
	::Effekseer::ModelLoader* CreateModelLoader( ::Effekseer::FileInterface* fileInterface = NULL )override;

	/**
	@brief	背景を取得する。
	*/
	Effekseer::TextureData* GetBackground() override 
	{
		if (m_background.UserID == 0) return nullptr;
		return &m_background;
	}

	/**
	@brief	背景を設定する。
	*/
	void SetBackground(GLuint background) override;

	EffekseerRenderer::DistortingCallback* GetDistortingCallback() override;

	void SetDistortingCallback(EffekseerRenderer::DistortingCallback* callback) override;

	/**
	@brief	描画モードを設定する。
	*/
	void SetRenderMode( Effekseer::RenderMode renderMode ) override
	{
		m_renderMode = renderMode;
	}
	Effekseer::RenderMode GetRenderMode() override
	{
		return m_renderMode;
	}

	EffekseerRenderer::StandardRenderer<RendererImplemented, Shader, Vertex, VertexDistortion>* GetStandardRenderer() { return m_standardRenderer; }

	void SetVertexBuffer( VertexBuffer* vertexBuffer, int32_t size );
	void SetVertexBuffer(GLuint vertexBuffer, int32_t size);
	void SetIndexBuffer( IndexBuffer* indexBuffer );
	void SetIndexBuffer(GLuint indexBuffer);
	void SetVertexArray( VertexArray* vertexArray );

	void SetLayout(Shader* shader);
	void DrawSprites( int32_t spriteCount, int32_t vertexOffset );
	void DrawPolygon( int32_t vertexCount, int32_t indexCount);

	Shader* GetShader(bool useTexture, bool useDistortion) const;
	void BeginShader(Shader* shader);
	void EndShader(Shader* shader);

	void SetVertexBufferToShader(const void* data, int32_t size);

	void SetPixelBufferToShader(const void* data, int32_t size);

	void SetTextures(Shader* shader, Effekseer::TextureData** textures, int32_t count);

	void ResetRenderState() override;

	int32_t GetDrawCallCount() const override;

	int32_t GetDrawVertexCount() const override;

	void ResetDrawCallCount() override;

	void ResetDrawVertexCount() override;

	std::vector<GLuint>& GetCurrentTextures() { return m_currentTextures; }

	OpenGLDeviceType GetDeviceType() { return m_deviceType; }

	virtual int GetRef() override { return ::Effekseer::ReferenceObject::GetRef(); }
	virtual int AddRef() override { return ::Effekseer::ReferenceObject::AddRef(); }
	virtual int Release() override { return ::Effekseer::ReferenceObject::Release(); }

private:
	void GenerateIndexData();
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRENDERER_GL_RENDERER_IMPLEMENTED_H__
