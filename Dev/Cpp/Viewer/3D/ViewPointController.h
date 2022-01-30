#pragma once

#include "../Math/Vector2I.h"
#include "../Math/Vector3F.h"

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

enum class CoordinateSystemType
{
	RH,
	LH,
};

struct Ray
{
	Vector3F Origin;
	Vector3F Direction;
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

	bool mouseRotDirectionInvX_ = false;
	bool mouseRotDirectionInvY_ = false;

	bool mouseSlideDirectionInvX_ = false;
	bool mouseSlideDirectionInvY_ = false;

	float m_orthoScale = 1.0f;
	ProjectionType projectionType_ = ProjectionType::Perspective;
	Effekseer::Matrix44 m_cameraMat;
	Effekseer::Matrix44 m_projMat;
	ProjectionMatrixStyle projectionStyle_ = ProjectionMatrixStyle::DirectXStyle;

	int32_t screenWidth = 0;
	int32_t screenHeight = 0;

	::Effekseer::Vector3D focusPosition_;

	CoordinateSystemType coordinateSystem_ = CoordinateSystemType::RH;

	void SetZoom(float zoom)
	{
		zoom_ = Effekseer::Max(MinZoom, Effekseer::Min(MaxZoom, zoom));
	}

	float GetOrthoScale();

public:
#if !defined(SWIG)
	float angleX_ = 30.0f;
	float angleY_ = -30.0f;
	float zoom_ = 0.0f;

	void Initialize(ProjectionMatrixStyle style, int width, int height);

	void RecalcProjection();

	void SetPerspectiveFov(int width, int height);

	void SetOrthographic(int width, int height);

	void SetOrthographicScale(float scale);

	float RateOfMagnification = 1.0f;

	float ClippingStart = 1.0f;

	float ClippingEnd = 300.0f;

	void SetScreenSize(int32_t width, int32_t height);

	Effekseer::Tool::Vector2I GetScreenSize() const
	{
		return Effekseer::Tool::Vector2I(screenWidth, screenHeight);
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

	void Update();
#endif

	Ray GetCameraRay() const;

	bool Rotate(float x, float y);

	bool Slide(float x, float y);

	bool Zoom(float delta);

	void SetMouseInverseFlag(bool rotX, bool rotY, bool slideX, bool slideY);

	ProjectionType GetProjectionType() const;

	void SetProjectionType(ProjectionType type);

	CoordinateSystemType GetCoordinateSystem() const;

	void SetCoordinateSystem(CoordinateSystemType type);

	Vector3F GetFocusPosition() const;

	void SetFocusPosition(const Vector3F& position);

	float GetDistance() const;

	void SetDistance(float distance);
};

} // namespace Effekseer::Tool