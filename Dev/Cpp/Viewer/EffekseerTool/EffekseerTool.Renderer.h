
#ifndef	__EFFEKSEERTOOL_RENDERER_H__
#define	__EFFEKSEERTOOL_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerTool.Base.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace EffekseerTool
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class Renderer
{
private:
	class DistortingCallback
		: public EffekseerRenderer::DistortingCallback
	{
	private:
		Renderer* renderer = nullptr;
	public:
		DistortingCallback(Renderer* renderer);
		virtual ~DistortingCallback();

		void OnDistorting();
	};

private:
	HWND				m_handle;
	int32_t				m_width;
	int32_t				m_height;
	LPDIRECT3D9			m_d3d;
	LPDIRECT3DDEVICE9	m_d3d_device;

	int32_t				m_squareMaxCount;

	eProjectionType		m_projection;

	::EffekseerRendererDX9::RendererImplemented*	m_renderer;

	::EffekseerRenderer::Grid*	m_grid;
	::EffekseerRenderer::Guide*	m_guide;
	::EffekseerRenderer::Culling*	m_culling;
	::EffekseerRenderer::Paste*	m_background;

	bool		m_recording;

	IDirect3DSurface9*	m_recordingTarget;
	IDirect3DTexture9*	m_recordingTargetTexture;
	IDirect3DSurface9*	m_recordingDepth;
	int32_t				m_recordingWidth;
	int32_t				m_recordingHeight;

	IDirect3DSurface9*	m_recordingTempTarget;
	IDirect3DSurface9*	m_recordingTempDepth;

	IDirect3DTexture9*	m_backGroundTexture;

	IDirect3DSurface9*	m_renderTarget = nullptr;
	IDirect3DTexture9*	m_renderTargetTexture = nullptr;
	IDirect3DSurface9*	m_renderTargetDepth = nullptr;

	IDirect3DSurface9*	m_renderEffectBackTarget = nullptr;
	IDirect3DTexture9*	m_renderEffectBackTargetTexture = nullptr;

	IDirect3DSurface9*	m_renderDefaultTarget = nullptr;
	IDirect3DSurface9*	m_renderDefaultDepth = nullptr;

	Effekseer::Matrix44	m_cameraMatTemp;
	Effekseer::Matrix44	m_projMatTemp;


	void GenerateRenderTargets(int32_t width, int32_t height);
public:
	/**
		@brief	コンストラクタ
	*/
	Renderer( int32_t squareMaxCount );

	/**
		@brief	デストラクタ
	*/
	~Renderer();

	/**
		@brief	初期化を行う。
	*/
	bool Initialize( HWND handle, int width, int height );

	/**
		@brief	デバイスを取得する。
	*/
	LPDIRECT3DDEVICE9 GetDevice();

	/**
		@brief	画面に表示する。
	*/
	bool Present();

	/**
		@brief	デバイスのリセット
	*/
	void ResetDevice();

	/**
		@brief	射影取得
	*/
	eProjectionType GetProjectionType();

	/**
		@brief	射影設定
	*/
	void SetProjectionType( eProjectionType type );

	/**
		@brief	画面サイズ変更
	*/
	bool Resize( int width, int height );

	void RecalcProjection();

	::EffekseerRenderer::Renderer*	GetRenderer() { return m_renderer; };

	/**
		@brief	射影行列設定
	*/
	void SetPerspectiveFov( int width, int height );

	/**
		@brief	射影行列設定
	*/
	void SetOrthographic( int width, int height );

	/**
		@brief	Orthographic表示の拡大率
	*/
	float	RateOfMagnification;

	/**
		@brief	ガイドの縦幅
	*/
	int32_t GuideWidth;

	/**
		@brief	ガイドの横幅
	*/
	int32_t	GuideHeight;

	/**
		@brief	ガイドを描画するかどうか
	*/
	bool RendersGuide;

	/**
		@brief	グリッドを表示するか?
	*/
	bool IsGridShown;

	/**
		@brief	XYグリッドを表示するか?
	*/
	bool IsGridXYShown;

	/**
		@brief	XZグリッドを表示するか?
	*/
	bool IsGridXZShown;

	/**
		@brief	YZグリッドを表示するか?
	*/
	bool IsGridYZShown;

	/**
		@brief	右手系か?
	*/
	bool IsRightHand;

	/**
		@brief	グリッドの長さ
	*/
	float GridLength;

	bool IsCullingShown;

	float CullingRadius;

	Effekseer::Vector3D CullingPosition;

	/**
		@brief	背景色
	*/
	Effekseer::Color GridColor;

	/**
		@brief	背景色
	*/
	Effekseer::Color BackgroundColor;

	/**
		@brief	背景が半透明か?
	*/
	bool IsBackgroundTranslucent;

	/**
		@brief	描画開始
	*/
	bool BeginRendering();

	/**
		@brief	描画終了
	*/
	bool EndRendering();

	/**
		@brief	描画中の背景をテクスチャとして背景を出力する。
	*/
	IDirect3DTexture9* ExportBackground();

	/**
		@brief	録画開始
	*/
	bool BeginRecord( int32_t width, int32_t height );

	/**
		@brief	録画位置変更
	*/
	void SetRecordRect( int32_t x, int32_t y );

	/**
		@brief	録画終了
	*/
	void EndRecord( const wchar_t* outputPath );

	/**
	@brief	録画終了
	*/
	void EndRecord(std::vector<Effekseer::Color>& pixels);

	/**
		@brief	背景の読み込み
	*/
	void LoadBackgroundImage( void* data, int32_t size );
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERTOOL_RENDERER_H__