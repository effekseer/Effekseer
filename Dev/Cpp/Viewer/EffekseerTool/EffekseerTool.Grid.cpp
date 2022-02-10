
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
Grid::Grid(Effekseer::Backend::GraphicsDeviceRef graphicsDevice)
	: m_lineCount(0)
	, m_gridLength(2.0f)
	, IsShownXY(false)
	, IsShownXZ(true)
	, IsShownYZ(false)
{
	lineRenderer_ = std::make_shared<Effekseer::Tool::LineRenderer>(graphicsDevice);
}

Grid* Grid::Create(Effekseer::Backend::GraphicsDeviceRef graphicsDevice)
{
	return new Grid(graphicsDevice);
}

void Grid::Rendering(const ::Effekseer::Color& gridColor, bool isRightHand, Effekseer::Matrix44 cameraMatrix, Effekseer::Matrix44 projectionMatrix)
{
	lineRenderer_->ClearCache();

	lineRenderer_->DrawLine(
		::Effekseer::Vector3D(0.0f, 0.0f, 0.0f), ::Effekseer::Vector3D(m_gridLength, 0.0f, 0.0f), ::Effekseer::Color(255, 0, 0, 255));

	lineRenderer_->DrawLine(
		::Effekseer::Vector3D(0.0f, 0.0f, 0.0f), ::Effekseer::Vector3D(0.0f, m_gridLength, 0.0f), ::Effekseer::Color(0, 255, 0, 255));

	lineRenderer_->DrawLine(
		::Effekseer::Vector3D(0.0f, 0.0f, 0.0f), ::Effekseer::Vector3D(0.0f, 0.0f, m_gridLength), ::Effekseer::Color(0, 0, 255, 255));

	if (IsShownXZ)
	{
		for (int i = -5; i <= 5; i++)
		{
			lineRenderer_->DrawLine(::Effekseer::Vector3D(i * m_gridLength, 0.0f, m_gridLength * 5.0f),
									::Effekseer::Vector3D(i * m_gridLength, 0.0f, -m_gridLength * 5.0f),
									gridColor);

			lineRenderer_->DrawLine(::Effekseer::Vector3D(m_gridLength * 5.0f, 0.0f, i * m_gridLength),
									::Effekseer::Vector3D(-m_gridLength * 5.0f, 0.0f, i * m_gridLength),
									gridColor);
		}
	}

	if (IsShownXY)
	{
		for (int i = -5; i <= 5; i++)
		{
			lineRenderer_->DrawLine(::Effekseer::Vector3D(i * m_gridLength, m_gridLength * 5.0f, 0.0f),
									::Effekseer::Vector3D(i * m_gridLength, -m_gridLength * 5.0f, 0.0f),
									gridColor);

			lineRenderer_->DrawLine(::Effekseer::Vector3D(m_gridLength * 5.0f, i * m_gridLength, 0.0f),
									::Effekseer::Vector3D(-m_gridLength * 5.0f, i * m_gridLength, 0.0f),
									gridColor);
		}
	}

	if (IsShownYZ)
	{
		for (int i = -5; i <= 5; i++)
		{
			lineRenderer_->DrawLine(::Effekseer::Vector3D(0.0f, i * m_gridLength, m_gridLength * 5.0f),
									::Effekseer::Vector3D(0.0f, i * m_gridLength, -m_gridLength * 5.0f),
									gridColor);

			lineRenderer_->DrawLine(::Effekseer::Vector3D(0.0f, m_gridLength * 5.0f, i * m_gridLength),
									::Effekseer::Vector3D(0.0f, -m_gridLength * 5.0f, i * m_gridLength),
									gridColor);
		}
	}

	lineRenderer_->DrawLine(
		::Effekseer::Vector3D(0.0f, 0.0f, 0.0f), ::Effekseer::Vector3D(m_gridLength, 0.0f, 0.0f), ::Effekseer::Color(255, 0, 0, 255));

	lineRenderer_->DrawLine(
		::Effekseer::Vector3D(0.0f, 0.0f, 0.0f), ::Effekseer::Vector3D(0.0f, m_gridLength, 0.0f), ::Effekseer::Color(0, 255, 0, 255));

	lineRenderer_->DrawLine(
		::Effekseer::Vector3D(0.0f, 0.0f, 0.0f), ::Effekseer::Vector3D(0.0f, 0.0f, m_gridLength), ::Effekseer::Color(0, 0, 255, 255));

	lineRenderer_->Render(cameraMatrix, projectionMatrix);
}

} // namespace EffekseerRenderer
