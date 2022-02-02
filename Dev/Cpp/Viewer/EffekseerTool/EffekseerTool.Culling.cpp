
#include "EffekseerTool.Culling.h"
#include <math.h>

namespace EffekseerRenderer
{

Culling::Culling(Effekseer::Backend::GraphicsDeviceRef graphicsDevice)
	: IsShown(false)
	, Radius(0.0f)
	, X(0.0f)
	, Y(0.0f)
	, Z(0.0f)
{
	lineRenderer_ = std::make_shared<Effekseer::Tool::LineRenderer>(graphicsDevice);
}

Culling* Culling::Create(Effekseer::Backend::GraphicsDeviceRef graphicsDevice)
{
	return new Culling(graphicsDevice);
}

void Culling::Rendering(bool isRightHand, Effekseer::Matrix44 cameraMatrix, Effekseer::Matrix44 projectionMatrix)
{
	if (!IsShown)
		return;

	lineRenderer_->ClearCache();

	for (int32_t y = -3; y <= 3; y++)
	{
		float ylen = Radius * ((float)y / 4.0f);
		float radius = sqrt(Radius * Radius - ylen * ylen);

		for (int32_t r = 0; r < 9; r++)
		{
			float a0 = 3.1415f * 2.0f / 9.0f * r;
			float a1 = 3.1415f * 2.0f / 9.0f * (r + 1.0f);

			lineRenderer_->DrawLine(::Effekseer::Vector3D(X + sin(a0) * radius, Y + ylen, Z + cos(a0) * radius),
									::Effekseer::Vector3D(X + sin(a1) * radius, Y + ylen, Z + cos(a1) * radius),
									::Effekseer::Color(255, 255, 255, 255));
		}
	}

	lineRenderer_->Render(cameraMatrix, projectionMatrix);
}

} // namespace EffekseerRenderer
