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

enum class CoordinateType
{
	RH,
	LH,
};

struct Ray
{
	Vector3D Origin;
	Vector3D Direction;
};

class ViewPointController
{
private:
	const float DistanceBase = 15.0f;
	const float OrthoScaleBase = 16.0f;
	const float ZoomDistanceFactor = 1.125f;
	const float MaxZoom = 40.0f;
	const float MinZoom = -40.0f;
	const float PI = 3.14159265f;

	bool g_mouseRotDirectionInvX = false;
	bool g_mouseRotDirectionInvY = false;

	bool g_mouseSlideDirectionInvX = false;
	bool g_mouseSlideDirectionInvY = false;

	float m_orthoScale = 1.0f;
	ProjectionType m_projection;
	Effekseer::Matrix44 m_cameraMat;
	Effekseer::Matrix44 m_projMat;
	ProjectionMatrixStyle projectionStyle_;

	int32_t screenWidth = 0;
	int32_t screenHeight = 0;

	float g_RotX = 30.0f;
	float g_RotY = -30.0f;
	float g_Zoom = 0.0f;
	::Effekseer::Vector3D g_focus_position;

	void SetZoom(float zoom)
	{
		g_Zoom = Effekseer::Max(MinZoom, Effekseer::Min(MaxZoom, zoom));
	}

	float GetDistance() const;

	float GetOrthoScale();

public:
	ViewPointController();

	~ViewPointController();

	void SetMouseInverseFlag(bool rotX, bool rotY, bool slideX, bool slideY);

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

	bool Rotate(float x, float y);

	bool Slide(float x, float y);

	bool Zoom(float zoom);

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

	Ray GetCameraRay() const;

	void Update();
};

} // namespace Effekseer::Tool