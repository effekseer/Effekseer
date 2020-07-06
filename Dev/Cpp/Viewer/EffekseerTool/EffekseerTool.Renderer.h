
#pragma once

#include "EffekseerTool.Base.h"

#ifdef _WIN32
#include "../Graphics/Platform/DX11/efk.GraphicsDX11.h"
#endif

#include "../Graphics/Platform/GL/efk.GraphicsGL.h"
#include "../Graphics/efk.PostEffects.h"
#include "../RenderedEffectGenerator.h"
#include "../efk.Base.h"

#include <functional>

namespace EffekseerTool
{

class MainScreenRenderedEffectGenerator : public Effekseer::Tool::RenderedEffectGenerator
{
private:
	std::shared_ptr<::EffekseerRenderer::Grid> grid_;
	std::shared_ptr<::EffekseerRenderer::Guide> guide_;
	std::shared_ptr<::EffekseerRenderer::Culling> culling_;
	std::shared_ptr<::EffekseerRenderer::Paste> background_;
	Effekseer::TextureLoader* textureLoader_ = nullptr;
	Effekseer::TextureData* backgroundData_ = nullptr;
	std::u16string backgroundPath;

public:
	virtual ~MainScreenRenderedEffectGenerator();

	bool InitializedPrePost();

	void OnAfterClear() override;

	void OnBeforePostprocess() override;

	Effekseer::Color GridColor = Effekseer::Color(255, 255, 255, 255);

	int32_t GuideWidth = 100;

	int32_t GuideHeight = 100;

	bool RendersGuide = false;

	bool IsGridShown = true;

	bool IsGridXYShown = false;

	bool IsGridXZShown = true;

	bool IsGridYZShown = false;

	float GridLength = 2.0f;

	bool IsCullingShown = false;

	float CullingRadius = 0.0f;

	bool IsRightHand = true;

	Effekseer::Vector3D CullingPosition;

	void LoadBackgroundImage(const char16_t* path);
};

class Renderer
{
private:
	float m_orthoScale = 1.0f;
	eProjectionType m_projection;
	Effekseer::Matrix44 m_cameraMat;
	Effekseer::Matrix44 m_projMat;
	efk::DeviceType deviceType_;

	int32_t screenWidth = 0;
	int32_t screenHeight = 0;

public:
	/**
		@brief	Constructor
	*/
	Renderer(efk::DeviceType deviceType);

	/**
		@brief	デストラクタ
	*/
	~Renderer();

	/**
		@brief	初期化を行う。
	*/
	bool Initialize(int width, int height);

	/**
		@brief	射影取得
	*/
	eProjectionType GetProjectionType();

	/**
		@brief	射影設定
	*/
	void SetProjectionType(eProjectionType type);

	void RecalcProjection();

	/**
		@brief	射影行列設定
	*/
	void SetPerspectiveFov(int width, int height);

	/**
		@brief	射影行列設定
	*/
	void SetOrthographic(int width, int height);

	/**
		@brief	射影行列設定
	*/
	void SetOrthographicScale(float scale);

	/**
		@brief	Orthographic表示の拡大率
	*/
	float RateOfMagnification;

	/**
		@brief	Z near
	*/
	float ClippingStart = 1.0f;

	/**
		@brief	Z far
	*/
	float ClippingEnd = 300.0f;

	/**
		@brief	右手系か?
	*/
	bool IsRightHand;

	/**
		@brief	The type of distortion
	*/
	Effekseer::Tool::DistortionType Distortion;

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

	void SetScreenSize(int32_t width, int32_t height);

	Effekseer::Tool::Vector2DI GetScreenSize() const
	{
		return Effekseer::Tool::Vector2DI(screenWidth, screenHeight);
	}

	Effekseer::Matrix44 GetCameraMatrix() const
	{
		return m_cameraMat;
	}

	void SetCameraMatrix(Effekseer::Matrix44 value)
	{
		m_cameraMat = value;
	}

	Effekseer::Matrix44 GetProjectionMatrix() const
	{
		return m_projMat;
	}
};

} // namespace EffekseerTool
