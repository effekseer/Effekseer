
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <EffekseerRenderer/EffekseerRendererDX9.Renderer.h>
#include <EffekseerRenderer/EffekseerRendererDX9.VertexBuffer.h>
#include <EffekseerRenderer/EffekseerRendererDX9.IndexBuffer.h>
#include <EffekseerRenderer/EffekseerRendererDX9.Shader.h>
#include <EffekseerRenderer/EffekseerRendererDX9.RenderState.h>
#include "EffekseerTool.Grid.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Grid::Grid( EffekseerRendererDX9::RendererImplemented* renderer)
	: DeviceObject( renderer )
	, m_renderer	( renderer )
	, m_lineCount		( 0 )
	, m_gridLength		( 2.0f )
	, IsShownXY	( false )
	, IsShownXZ	( true )
	, IsShownYZ	( false )
{
	lineRenderer = new efk::LineRendererDX9(renderer);
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
Grid* Grid::Create( EffekseerRendererDX9::RendererImplemented* renderer )
{
	assert( renderer != NULL );
	assert( renderer->GetDevice() != NULL );

	return new Grid( renderer);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Grid::OnLostDevice()
{
	lineRenderer->OnLostDevice();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Grid::OnResetDevice()
{
	lineRenderer->OnResetDevice();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Grid::Rendering( ::Effekseer::Color& gridColor, bool isRightHand )
{
	lineRenderer->ClearCache();

	if (IsShownXZ)
	{
		for (int i = -5; i <= 5; i++)
		{
			lineRenderer->DrawLine(
				::Effekseer::Vector3D(i * m_gridLength, 0.0f, m_gridLength * 5.0f),
				::Effekseer::Vector3D(i * m_gridLength, 0.0f, -m_gridLength * 5.0f),
				gridColor);

			lineRenderer->DrawLine(
				::Effekseer::Vector3D(m_gridLength * 5.0f, 0.0f, i * m_gridLength),
				::Effekseer::Vector3D(-m_gridLength * 5.0f, 0.0f, i * m_gridLength),
				gridColor);
		}
	}

	if (IsShownXY)
	{
		for (int i = -5; i <= 5; i++)
		{
			lineRenderer->DrawLine(
				::Effekseer::Vector3D(i * m_gridLength, m_gridLength * 5.0f, 0.0f),
				::Effekseer::Vector3D(i * m_gridLength, -m_gridLength * 5.0f, 0.0f),
				gridColor);

			lineRenderer->DrawLine(
				::Effekseer::Vector3D(m_gridLength * 5.0f, i * m_gridLength, 0.0f),
				::Effekseer::Vector3D(-m_gridLength * 5.0f, i * m_gridLength, 0.0f),
				gridColor);
		}
	}

	if (IsShownYZ)
	{
		for (int i = -5; i <= 5; i++)
		{
			lineRenderer->DrawLine(
				::Effekseer::Vector3D(0.0f, i * m_gridLength, m_gridLength * 5.0f),
				::Effekseer::Vector3D(0.0f, i * m_gridLength, -m_gridLength * 5.0f),
				gridColor);

			lineRenderer->DrawLine(
				::Effekseer::Vector3D(0.0f, m_gridLength * 5.0f, i * m_gridLength),
				::Effekseer::Vector3D(0.0f, -m_gridLength * 5.0f, i * m_gridLength),
				gridColor);
		}
	}


	lineRenderer->DrawLine(
		::Effekseer::Vector3D(0.0f, 0.0f, 0.0f),
		::Effekseer::Vector3D(m_gridLength, 0.0f, 0.0f),
		::Effekseer::Color(255, 0, 0, 255));

	lineRenderer->DrawLine(
		::Effekseer::Vector3D(0.0f, 0.0f, 0.0f),
		::Effekseer::Vector3D(0.0f, m_gridLength, 0.0f),
		::Effekseer::Color(0, 255, 0, 255));

	lineRenderer->DrawLine(
		::Effekseer::Vector3D(0.0f, 0.0f, 0.0f),
		::Effekseer::Vector3D(0.0f, 0.0f, m_gridLength),
		::Effekseer::Color(0, 0, 255, 255));

	lineRenderer->Render();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
