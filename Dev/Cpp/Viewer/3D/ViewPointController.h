#pragma once

#include "../Math/Vector2DI.h"
#include <Effekseer.h>

namespace Effekseer::Tool
{

enum class ProjectionMatrixStyle
{
	DirectXStyle,
	OpenGLStyle,
};

enum class ProjectionType
{
	Perspective,
	Orthographic,
};

class ViewPointController
{
private:
	float m_orthoScale = 1.0f;
	ProjectionType m_projection;
	Effekseer::Matrix44 m_cameraMat;
	Effekseer::Matrix44 m_projMat;
	ProjectionMatrixStyle projectionStyle_;

	int32_t screenWidth = 0;
	int32_t screenHeight = 0;

public:
	ViewPointController();

	~ViewPointController();

	void Initialize(ProjectionMatrixStyle style, int width, int height);

	ProjectionType GetProjectionType();

	void SetProjectionType(ProjectionType type);

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

} // namespace Effekseer::Tool