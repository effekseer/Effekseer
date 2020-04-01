
#pragma once

#include "EffekseerTool.Base.h"

#ifdef _WIN32
#include "../Graphics/Platform/DX11/efk.GraphicsDX11.h"
#endif

#include "../Graphics/Platform/GL/efk.GraphicsGL.h"
#include "../efk.Base.h"
#include "../Graphics/efk.PostEffects.h"

#include <functional>

namespace EffekseerTool
{

class Renderer
{
private:
	class DistortingCallback
		: public EffekseerRenderer::DistortingCallback
	{
	private:
		efk::Graphics* renderer = nullptr;
	public:
		DistortingCallback(efk::Graphics* renderer);
		virtual ~DistortingCallback();

		bool OnDistorting();

		bool IsEnabled;
		bool Blit;
	};

private:
	efk::Graphics*	graphics = nullptr;

	bool isScreenMode = false;

	int32_t				currentWidth = 0;
	int32_t				currentHeight = 0;
	
	int32_t				m_windowWidth = 0;
	int32_t				m_windowHeight = 0;
	int32_t				m_squareMaxCount;
	float				m_orthoScale = 1.0f;

	eProjectionType		m_projection;

	::EffekseerRenderer::Renderer*	m_renderer;
	DistortingCallback*		m_distortionCallback;

	::EffekseerRenderer::Grid*	m_grid;
	::EffekseerRenderer::Guide*	m_guide;
	::EffekseerRenderer::Culling*	m_culling;
	::EffekseerRenderer::Paste*	m_background;
	std::unique_ptr<efk::BloomEffect> m_bloomEffect;
	std::unique_ptr<efk::TonemapEffect> m_tonemapEffect;
	std::unique_ptr<efk::LinearToSRGBEffect> m_linearToSRGBEffect;

	bool		m_recording = false;
	int32_t		m_recordingWidth = 0;
	int32_t		m_recordingHeight = 0;

	Effekseer::TextureLoader*	textureLoader = nullptr;
	Effekseer::TextureData*		backgroundData = nullptr;

	Effekseer::Matrix44	m_cameraMatTemp;
	Effekseer::Matrix44	m_projMatTemp;

	std::u16string	backgroundPath;

	bool	m_isSRGBMode = false;

	std::shared_ptr<efk::RenderTexture>	viewRenderTexture;
	std::shared_ptr<efk::DepthTexture>	viewDepthTexture;

	//! a render texture which is drawn at last
	efk::RenderTexture* lastDstRenderTexture = nullptr;

	//! a depth texture which is drawn at last
	efk::DepthTexture* lastDstDepthTexture = nullptr;

	std::shared_ptr<efk::RenderTexture> linearRenderTexture;
	std::shared_ptr<efk::RenderTexture> hdrRenderTexture;
	std::shared_ptr<efk::RenderTexture> hdrRenderTextureMSAA;
	std::shared_ptr<efk::DepthTexture> depthTexture;
	
	int32_t		screenWidth = 0;
	int32_t		screenHeight = 0;
	uint32_t	msaaSamples = 4;
public:
	/**
		@brief	Constructor
	*/
	Renderer(int32_t squareMaxCount, bool isSRGBMode, efk::DeviceType deviceType);

	/**
		@brief	デストラクタ
	*/
	~Renderer();

	/**
		@brief	初期化を行う。
	*/
	bool Initialize( void* handle, int width, int height );

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
		@brief	射影行列設定
	*/
	void SetOrthographicScale( float scale );

	/**
		@brief	Orthographic表示の拡大率
	*/
	float	RateOfMagnification;

	/**
		@brief	Z near
	*/
	float ClippingStart = 1.0f;

	/**
		@brief	Z far
	*/
	float ClippingEnd = 300.0f;

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
		@brief	The type of distortion
	*/
	DistortionType Distortion;

	/**
		@brief	背景色
	*/
	Effekseer::Color GridColor;

	/**
		@brief	背景色
	*/
	Effekseer::Color BackgroundColor;

	/**
	@brief	背景色
	*/
	Effekseer::RenderMode RenderingMode;

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

	bool BeginRenderToView(int32_t width, int32_t height);

	bool EndRenderToView();

	void RenderPostEffect();

	/**
		@brief	録画開始
	*/
	bool BeginRecord( int32_t width, int32_t height );

	/**
	@brief	録画終了
	*/
	void EndRecord(std::vector<Effekseer::Color>& pixels, bool generateAlpha, bool removeAlpha);

	/**
		@brief	背景の読み込み
	*/
	void LoadBackgroundImage(const char16_t* path);

	/**
		@brief	copy current render target to background buffer
	*/
	void CopyToBackground();

	/**
		@brief	temp
	*/
	uint64_t GetViewID();

	efk::Graphics* GetGraphics() const { return graphics; }

	efk::BloomEffect* GetBloomEffect() const { return m_bloomEffect.get(); }
	efk::TonemapEffect* GetTonemapEffect() const { return m_tonemapEffect.get(); }

	/**
		Called when device is losted.
	*/
	std::function<void()>	LostedDevice;

	/**
	Called when device is resetted.
	*/
	std::function<void()>	ResettedDevice;

};

}
