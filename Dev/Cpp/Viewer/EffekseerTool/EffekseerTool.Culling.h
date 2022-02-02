
#pragma once

#include "../Graphics/LineRenderer.h"

namespace EffekseerRenderer
{

class Culling
{
private:
	std::shared_ptr<Effekseer::Tool::LineRenderer> lineRenderer_;
	Culling(Effekseer::Backend::GraphicsDeviceRef graphicsDevice);

public:
	static Culling* Create(Effekseer::Backend::GraphicsDeviceRef graphicsDevice);

public:
	void Rendering(bool isRightHand, Effekseer::Matrix44 cameraMatrix, Effekseer::Matrix44 projectionMatrix);

	bool IsShown;
	float Radius;
	float X;
	float Y;
	float Z;
};

} // namespace EffekseerRenderer
