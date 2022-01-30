#include "ViewPointController.h"

namespace Effekseer::Tool
{

float ViewPointController::GetOrthoScale()
{
	return OrthoScaleBase / powf(ZoomDistanceFactor, zoom_);
}

void ViewPointController::SetPerspectiveFov(int width, int height)
{
	::Effekseer::Matrix44 proj;

	if (ProjectionStyle == ProjectionMatrixStyle::OpenGLStyle)
	{
		if (coordinateSystem_ == CoordinateSystemType::RH)
		{
			proj.PerspectiveFovRH_OpenGL(60.0f / 180.0f * 3.141592f, (float)width / (float)height, ClippingStart, ClippingEnd);
		}
		else
		{
			proj.PerspectiveFovLH_OpenGL(60.0f / 180.0f * 3.141592f, (float)width / (float)height, ClippingStart, ClippingEnd);
		}
	}
	else
	{
		if (coordinateSystem_ == CoordinateSystemType::RH)
		{
			proj.PerspectiveFovRH(60.0f / 180.0f * 3.141592f, (float)width / (float)height, ClippingStart, ClippingEnd);
		}
		else
		{
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

	if (coordinateSystem_ == CoordinateSystemType::RH)
	{
		proj.OrthographicRH((float)width / m_orthoScale / RateOfMagnification,
							(float)height / m_orthoScale / RateOfMagnification,
							ClippingStart,
							ClippingEnd);
	}
	else
	{
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
	if (projectionType_ == ProjectionType::Perspective)
	{
		SetPerspectiveFov(screenWidth, screenHeight);
	}
	else if (projectionType_ == ProjectionType::Orthographic)
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

void ViewPointController::Update()
{
	const auto ray = GetCameraRay();

	if (coordinateSystem_ == CoordinateSystemType::RH)
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

	RecalcProjection();
}

Ray ViewPointController::GetCameraRay() const
{
	Ray ret;

	::Effekseer::Vector3D position(0, 0, GetDistance());
	::Effekseer::Matrix43 mat, mat_rot_x, mat_rot_y;
	mat_rot_x.RotationX(-angleX_ / 180.0f * PI);

	if (coordinateSystem_ == CoordinateSystemType::RH)
	{
		mat_rot_y.RotationY(-angleY_ / 180.0f * PI);
		::Effekseer::Matrix43::Multiple(mat, mat_rot_x, mat_rot_y);
		::Effekseer::Vector3D::Transform(position, position, mat);

		Effekseer::Vector3D direction;
		Effekseer::Vector3D::Normal(direction, -position);

		position.X += focusPosition_.X;
		position.Y += focusPosition_.Y;
		position.Z += focusPosition_.Z;

		ret.Direction = direction;
		ret.Origin = position;
	}
	else
	{
		mat_rot_y.RotationY((angleY_ + 180.0f) / 180.0f * PI);
		::Effekseer::Matrix43::Multiple(mat, mat_rot_x, mat_rot_y);
		::Effekseer::Vector3D::Transform(position, position, mat);

		::Effekseer::Vector3D temp_focus = focusPosition_;
		temp_focus.Z = -temp_focus.Z;

		Effekseer::Vector3D direction;

		Effekseer::Vector3D::Normal(direction, -position);

		position.X += temp_focus.X;
		position.Y += temp_focus.Y;
		position.Z += temp_focus.Z;

		ret.Direction = direction;
		ret.Origin = position;
	}

	return ret;
}

bool ViewPointController::Rotate(float x, float y)
{
	if (mouseRotDirectionInvX_)
	{
		x = -x;
	}

	if (mouseRotDirectionInvY_)
	{
		y = -y;
	}

	angleY_ += x;
	angleX_ += y;

	while (angleY_ >= 180.0f)
	{
		angleY_ -= 180.0f * 2.0f;
	}

	while (angleY_ <= -180.0f)
	{
		angleY_ += 180.0f * 2.0f;
	}

	if (angleX_ > 180.0f / 2.0f)
	{
		angleX_ = 180.0f / 2.0f;
	}

	if (angleX_ < -180.0f / 2.0f)
	{
		angleX_ = -180.0f / 2.0f;
	}

	return true;
}

bool ViewPointController::Slide(float x, float y)
{
	if (mouseSlideDirectionInvX_)
	{
		x = -x;
	}

	if (mouseSlideDirectionInvY_)
	{
		y = -y;
	}

	::Effekseer::Vector3D up(0, 1, 0);
	::Effekseer::Vector3D right(1, 0, 0);
	::Effekseer::Matrix43 mat, mat_rot_x, mat_rot_y;
	mat_rot_x.RotationX(-angleX_ / 180.0f * PI);
	mat_rot_y.RotationY(-angleY_ / 180.0f * PI);
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

	float moveFactor = powf(ZoomDistanceFactor, zoom_);
	focusPosition_.X += v.X * moveFactor;
	focusPosition_.Y += v.Y * moveFactor;
	focusPosition_.Z += v.Z * moveFactor;

	return true;
}

bool ViewPointController::Zoom(float delta)
{
	SetZoom(zoom_ - delta);
	return true;
}

void ViewPointController::SetMouseInverseFlag(bool rotX, bool rotY, bool slideX, bool slideY)
{
	mouseRotDirectionInvX_ = rotX;
	mouseRotDirectionInvY_ = rotY;

	mouseSlideDirectionInvX_ = slideX;
	mouseSlideDirectionInvY_ = slideY;
}

ProjectionType ViewPointController::GetProjectionType() const
{
	return projectionType_;
}

void ViewPointController::SetProjectionType(ProjectionType type)
{
	projectionType_ = type;

	RecalcProjection();
}

CoordinateSystemType ViewPointController::GetCoordinateSystem() const
{
	return coordinateSystem_;
}

void ViewPointController::SetCoordinateSystem(CoordinateSystemType type)
{
	coordinateSystem_ = type;

	RecalcProjection();
}

Vector3F ViewPointController::GetFocusPosition() const
{
	return focusPosition_;
}

void ViewPointController::SetFocusPosition(const Vector3F& position)
{
	focusPosition_ = position;
}

float ViewPointController::GetDistance() const
{
	return DistanceBase * powf(ZoomDistanceFactor, zoom_);
}

void ViewPointController::SetDistance(float distance)
{
	SetZoom(logf(Effekseer::Max(FLT_MIN, distance / DistanceBase)) / logf(ZoomDistanceFactor));
}

float ViewPointController::GetAngleX() const
{
	return angleX_;
}

void ViewPointController::SetAngleX(float value)
{
	angleX_ = value;
}

float ViewPointController::GetAngleY() const
{
	return angleY_;
}

void ViewPointController::SetAngleY(float value)
{
	angleY_ = value;
}

} // namespace Effekseer::Tool