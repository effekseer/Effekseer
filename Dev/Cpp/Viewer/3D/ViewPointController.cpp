#include "ViewPointController.h"

namespace Effekseer::Tool
{

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

} // namespace Effekseer::Tool