
#pragma once

#include "EffekseerTool.Base.h"

#include "../Graphics/StaticMeshRenderer.h"
#include "../RenderedEffectGenerator.h"
#include "../efk.Base.h"
#include <functional>
#include <string>

namespace EffekseerTool
{

class MainScreenRenderedEffectGenerator : public Effekseer::Tool::RenderedEffectGenerator
{
private:
	std::shared_ptr<::EffekseerRenderer::Grid> grid_;
	std::shared_ptr<::EffekseerRenderer::Guide> guide_;
	std::shared_ptr<::EffekseerRenderer::Culling> culling_;

	Effekseer::TextureLoaderRef textureLoader_;
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

class ViewPointController
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
	ViewPointController();

	~ViewPointController();

	void Initialize(efk::DeviceType deviceType, int width, int height);

	eProjectionType GetProjectionType();

	void SetProjectionType(eProjectionType type);

	void RecalcProjection();

	void SetPerspectiveFov(int width, int height);

	void SetOrthographic(int width, int height);

	void SetOrthographicScale(float scale);

	float RateOfMagnification;

	/**
		@brief	Z near
	*/
	float ClippingStart = 1.0f;

	/**
		@brief	Z far
	*/
	float ClippingEnd = 300.0f;

	bool IsRightHand;

	Effekseer::Tool::RenderingMethodType RenderingMode;

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
