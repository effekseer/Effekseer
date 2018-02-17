
#ifndef	__EFFEKSEERRENDERER_GRID_H__
#define	__EFFEKSEERRENDERER_GRID_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <EffekseerRenderer/EffekseerRendererDX9.Renderer.h>
#include <EffekseerRenderer/EffekseerRendererDX9.RendererImplemented.h>
#include <EffekseerRenderer/EffekseerRendererDX9.DeviceObject.h>

#include "../Graphics/Platform/DX9/efk.LineRendererDX9.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class Grid
	: public EffekseerRendererDX9::DeviceObject
{
private:
	
	struct Vertex
	{
		::Effekseer::Vector3D	Pos;
		float	Col[4];
	};

	EffekseerRendererDX9::RendererImplemented*			m_renderer;
	efk::LineRendererDX9*	lineRenderer = nullptr;

	int32_t							m_lineCount;
	float							m_gridLength;

	Grid( EffekseerRendererDX9::RendererImplemented* renderer);
public:

	virtual ~Grid();

	static Grid* Create( EffekseerRendererDX9::RendererImplemented* renderer );

public:	// デバイス復旧用
	virtual void OnLostDevice();
	virtual void OnResetDevice();

public:
	void Rendering( ::Effekseer::Color& gridColor, bool isRightHand );
	void SetLength( float gridLength ) { m_gridLength = gridLength; }

	bool IsShownXY;
	bool IsShownXZ;
	bool IsShownYZ;
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRENDERER_GRID_H__