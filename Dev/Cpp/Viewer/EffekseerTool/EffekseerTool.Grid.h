
#ifndef __EFFEKSEERRENDERER_GRID_H__
#define __EFFEKSEERRENDERER_GRID_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "../Graphics/efk.LineRenderer.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class Grid
{

	efk::LineRenderer* lineRenderer = nullptr;

	int32_t m_lineCount;
	float m_gridLength;

	Grid(efk::Graphics* graphics, EffekseerRenderer::Renderer* renderer);

public:
	virtual ~Grid();

	static Grid* Create(efk::Graphics* graphics, EffekseerRenderer::Renderer* renderer);

public:
	void Rendering(::Effekseer::Color& gridColor, bool isRightHand);
	void SetLength(float gridLength)
	{
		m_gridLength = gridLength;
	}

	bool IsShownXY;
	bool IsShownXZ;
	bool IsShownYZ;
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRenderer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_GRID_H__