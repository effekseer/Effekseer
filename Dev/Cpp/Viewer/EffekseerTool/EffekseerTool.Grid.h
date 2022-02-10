
#pragma once

#include "../Graphics/LineRenderer.h"

namespace EffekseerRenderer
{

class Grid
{
	std::shared_ptr<Effekseer::Tool::LineRenderer> lineRenderer_;

	int32_t m_lineCount;
	float m_gridLength;

	Grid(Effekseer::Backend::GraphicsDeviceRef graphicsDevice);

public:
	
	static Grid* Create(Effekseer::Backend::GraphicsDeviceRef graphicsDevice);

public:
	void Rendering(const ::Effekseer::Color& gridColor, bool isRightHand, Effekseer::Matrix44 cameraMatrix, Effekseer::Matrix44 projectionMatrix);
	void SetLength(float gridLength)
	{
		m_gridLength = gridLength;
	}

	bool IsShownXY;
	bool IsShownXZ;
	bool IsShownYZ;
};

} // namespace EffekseerRenderer
