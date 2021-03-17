
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerTool.Grid.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Grid::Grid(efk::Graphics* graphics, const EffekseerRenderer::RendererRef& renderer)
	: m_lineCount(0)
	, m_gridLength(2.0f)
	, IsShownXY(false)
	, IsShownXZ(true)
	, IsShownYZ(false)
{
	lineRenderer = efk::LineRenderer::Create(graphics, renderer);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Grid::~Grid()
{
	ES_SAFE_DELETE(lineRenderer);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Grid* Grid::Create(efk::Graphics* graphics, const EffekseerRenderer::RendererRef& renderer)
{
	return new Grid(graphics, renderer);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Grid::Rendering(::Effekseer::Color& gridColor, bool isRightHand)
{
	lineRenderer->ClearCache();

	lineRenderer->DrawLine(
		::Effekseer::Vector3D(0.0f, 0.0f, 0.0f), ::Effekseer::Vector3D(m_gridLength, 0.0f, 0.0f), ::Effekseer::Color(255, 0, 0, 255));

	lineRenderer->DrawLine(
		::Effekseer::Vector3D(0.0f, 0.0f, 0.0f), ::Effekseer::Vector3D(0.0f, m_gridLength, 0.0f), ::Effekseer::Color(0, 255, 0, 255));

	lineRenderer->DrawLine(
		::Effekseer::Vector3D(0.0f, 0.0f, 0.0f), ::Effekseer::Vector3D(0.0f, 0.0f, m_gridLength), ::Effekseer::Color(0, 0, 255, 255));

	if (IsShownXZ)
	{
		for (int i = -5; i <= 5; i++)
		{
			lineRenderer->DrawLine(::Effekseer::Vector3D(i * m_gridLength, 0.0f, m_gridLength * 5.0f),
								   ::Effekseer::Vector3D(i * m_gridLength, 0.0f, -m_gridLength * 5.0f),
								   gridColor);

			lineRenderer->DrawLine(::Effekseer::Vector3D(m_gridLength * 5.0f, 0.0f, i * m_gridLength),
								   ::Effekseer::Vector3D(-m_gridLength * 5.0f, 0.0f, i * m_gridLength),
								   gridColor);
		}
	}

	if (IsShownXY)
	{
		for (int i = -5; i <= 5; i++)
		{
			lineRenderer->DrawLine(::Effekseer::Vector3D(i * m_gridLength, m_gridLength * 5.0f, 0.0f),
								   ::Effekseer::Vector3D(i * m_gridLength, -m_gridLength * 5.0f, 0.0f),
								   gridColor);

			lineRenderer->DrawLine(::Effekseer::Vector3D(m_gridLength * 5.0f, i * m_gridLength, 0.0f),
								   ::Effekseer::Vector3D(-m_gridLength * 5.0f, i * m_gridLength, 0.0f),
								   gridColor);
		}
	}

	if (IsShownYZ)
	{
		for (int i = -5; i <= 5; i++)
		{
			lineRenderer->DrawLine(::Effekseer::Vector3D(0.0f, i * m_gridLength, m_gridLength * 5.0f),
								   ::Effekseer::Vector3D(0.0f, i * m_gridLength, -m_gridLength * 5.0f),
								   gridColor);

			lineRenderer->DrawLine(::Effekseer::Vector3D(0.0f, m_gridLength * 5.0f, i * m_gridLength),
								   ::Effekseer::Vector3D(0.0f, -m_gridLength * 5.0f, i * m_gridLength),
								   gridColor);
		}
	}

	lineRenderer->DrawLine(
		::Effekseer::Vector3D(0.0f, 0.0f, 0.0f), ::Effekseer::Vector3D(m_gridLength, 0.0f, 0.0f), ::Effekseer::Color(255, 0, 0, 255));

	lineRenderer->DrawLine(
		::Effekseer::Vector3D(0.0f, 0.0f, 0.0f), ::Effekseer::Vector3D(0.0f, m_gridLength, 0.0f), ::Effekseer::Color(0, 255, 0, 255));

	lineRenderer->DrawLine(
		::Effekseer::Vector3D(0.0f, 0.0f, 0.0f), ::Effekseer::Vector3D(0.0f, 0.0f, m_gridLength), ::Effekseer::Color(0, 0, 255, 255));

	lineRenderer->Render();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRenderer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
