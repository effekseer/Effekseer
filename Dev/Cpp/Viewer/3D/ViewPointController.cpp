#include "ViewPointController.h"

namespace Effekseer::Tool
{

void ViewPointController::SetDistance(float distance)
{
	SetZoom(logf(Effekseer::Max(FLT_MIN, distance / DistanceBase)) / logf(ZoomDistanceFactor));
}

float ViewPointController::GetDistance() const
{
	return DistanceBase * powf(ZoomDistanceFactor, g_Zoom);
}

float ViewPointController::GetOrthoScale()
{
	return OrthoScaleBase / powf(ZoomDistanceFactor, g_Zoom);
}

void ViewPointController::SetMouseInverseFlag(bool rotX, bool rotY, bool slideX, bool slideY)
{
	g_mouseRotDirectionInvX = rotX;
	g_mouseRotDirectionInvY = rotY;

	g_mouseSlideDirectionInvX = slideX;
	g_mouseSlideDirectionInvY = slideY;
}

void ViewPointController::Initialize(ProjectionMatrixStyle style, int width, int height)
{
	projectionStyle_ = style;
	screenWidth = width;
	screenHeight = height;

	RecalcProjection();
}

ProjectionType ViewPointController::GetProjectionType()
{
	return m_projection;
}

void ViewPointController::SetProjectionType(ProjectionType type)
{
	m_projection = type;

	RecalcProjection();
}

void ViewPointController::SetPerspectiveFov(int width, int height)
{
	::Effekseer::Matrix44 proj;

	if (projectionStyle_ == ProjectionMatrixStyle::OpenGLStyle)
	{
		if (IsRightHand)
		{
			// Right hand coordinate
			proj.PerspectiveFovRH_OpenGL(60.0f / 180.0f * 3.141592f, (float)width / (float)height, ClippingStart, ClippingEnd);
		}
		else
		{
			// Left hand coordinate
			proj.PerspectiveFovLH_OpenGL(60.0f / 180.0f * 3.141592f, (float)width / (float)height, ClippingStart, ClippingEnd);
		}
	}
	else
	{
		if (IsRightHand)
		{
			// Right hand coordinate
			proj.PerspectiveFovRH(60.0f / 180.0f * 3.141592f, (float)width / (float)height, ClippingStart, ClippingEnd);
		}
		else
		{
			// Left hand coordinate
			proj.PerspectiveFovLH(60.0f / 180.0f * 3.141592f, (float)width / (float)height, ClippingStart, ClippingEnd);
		}
	}

	proj.Values[0][0] *= RateOfMagnification;
	proj.Values[1][1] *= RateOfMagnification;

	m_projMat = proj;
}

void ViewPointController::SetOrthographic(int width, int height)
{
	::Effekseer::Matrix44 proj;

	if (IsRightHand)
	{
		// Right hand coordinate
		proj.OrthographicRH((float)width / m_orthoScale / RateOfMagnification,
							(float)height / m_orthoScale / RateOfMagnification,
							ClippingStart,
							ClippingEnd);
	}
	else
	{
		// Left hand coordinate
		proj.OrthographicLH((float)width / m_orthoScale / RateOfMagnification,
							(float)height / m_orthoScale / RateOfMagnification,
							ClippingStart,
							ClippingEnd);
	}

	m_projMat = proj;
}

void ViewPointController::SetOrthographicScale(float scale)
{
	m_orthoScale = scale;
}

void ViewPointController::RecalcProjection()
{
	if (m_projection == ProjectionType::Perspective)
	{
		SetPerspectiveFov(screenWidth, screenHeight);
	}
	else if (m_projection == ProjectionType::Orthographic)
	{
		SetOrthographic(screenWidth, screenHeight);
	}
}

void ViewPointController::SetScreenSize(int32_t width, int32_t height)
{
	screenWidth = width;
	screenHeight = height;

	RecalcProjection();
}

bool ViewPointController::Rotate(float x, float y)
{
	if (g_mouseRotDirectionInvX)
	{
		x = -x;
	}

	if (g_mouseRotDirectionInvY)
	{
		y = -y;
	}

	g_RotY += x;
	g_RotX += y;

	while (g_RotY >= 180.0f)
	{
		g_RotY -= 180.0f * 2.0f;
	}

	while (g_RotY <= -180.0f)
	{
		g_RotY += 180.0f * 2.0f;
	}

	if (g_RotX > 180.0f / 2.0f)
	{
		g_RotX = 180.0f / 2.0f;
	}

	if (g_RotX < -180.0f / 2.0f)
	{
		g_RotX = -180.0f / 2.0f;
	}

	return true;
}

bool ViewPointController::Slide(float x, float y)
{
	if (g_mouseSlideDirectionInvX)
	{
		x = -x;
	}

	if (g_mouseSlideDirectionInvY)
	{
		y = -y;
	}

	::Effekseer::Vector3D up(0, 1, 0);
	::Effekseer::Vector3D right(1, 0, 0);
	::Effekseer::Matrix43 mat, mat_rot_x, mat_rot_y;
	mat_rot_x.RotationX(-g_RotX / 180.0f * PI);
	mat_rot_y.RotationY(-g_RotY / 180.0f * PI);
	::Effekseer::Matrix43::Multiple(mat, mat_rot_x, mat_rot_y);
	::Effekseer::Vector3D::Transform(up, up, mat);
	::Effekseer::Vector3D::Transform(right, right, mat);

	up.X = up.X * y;
	up.Y = up.Y * y;
	up.Z = up.Z * y;
	right.X = right.X * (-x);
	right.Y = right.Y * (-x);
	right.Z = right.Z * (-x);

	::Effekseer::Vector3D v;
	v.X = up.X + right.X;
	v.Y = up.Y + right.Y;
	v.Z = up.Z + right.Z;

	float moveFactor = powf(ZoomDistanceFactor, g_Zoom);
	g_focus_position.X += v.X * moveFactor;
	g_focus_position.Y += v.Y * moveFactor;
	g_focus_position.Z += v.Z * moveFactor;

	return true;
}

bool ViewPointController::Zoom(float zoom)
{
	SetZoom(g_Zoom - zoom);
	return true;
}

Ray ViewPointController::GetCameraRay() const
{
	Ray ret;

	::Effekseer::Vector3D position(0, 0, GetDistance());
	::Effekseer::Matrix43 mat, mat_rot_x, mat_rot_y;
	mat_rot_x.RotationX(-g_RotX / 180.0f * PI);

	if (IsRightHand)
	{
		mat_rot_y.RotationY(-g_RotY / 180.0f * PI);
		::Effekseer::Matrix43::Multiple(mat, mat_rot_x, mat_rot_y);
		::Effekseer::Vector3D::Transform(position, position, mat);

		Effekseer::Vector3D::Normal(ret.Direction, -position);

		position.X += g_focus_position.X;
		position.Y += g_focus_position.Y;
		position.Z += g_focus_position.Z;

		ret.Origin = position;
	}
	else
	{
		mat_rot_y.RotationY((g_RotY + 180.0f) / 180.0f * PI);
		::Effekseer::Matrix43::Multiple(mat, mat_rot_x, mat_rot_y);
		::Effekseer::Vector3D::Transform(position, position, mat);

		::Effekseer::Vector3D temp_focus = g_focus_position;
		temp_focus.Z = -temp_focus.Z;

		Effekseer::Vector3D::Normal(ret.Direction, -position);

		position.X += temp_focus.X;
		position.Y += temp_focus.Y;
		position.Z += temp_focus.Z;

		ret.Origin = position;
	}

	return ret;
}

void ViewPointController::Update()
{
	const auto ray = GetCameraRay();

	if (IsRightHand)
	{
		::Effekseer::Matrix44 cameraMat;
		SetCameraMatrix(
			::Effekseer::Matrix44().LookAtRH(ray.Origin, ray.Origin + ray.Direction, ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f)));
	}
	else
	{
		::Effekseer::Matrix44 cameraMat;
		SetCameraMatrix(
			::Effekseer::Matrix44().LookAtLH(ray.Origin, ray.Origin + ray.Direction, ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f)));
	}

	SetOrthographicScale(GetOrthoScale());
}

} // namespace Effekseer::Tool