
#ifndef	__EFFEKSEERRENDERER_GRID_H__
#define	__EFFEKSEERRENDERER_GRID_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <EffekseerRenderer/EffekseerRendererDX9.Renderer.h>
#include <EffekseerRenderer/EffekseerRendererDX9.RendererImplemented.h>
#include <EffekseerRenderer/EffekseerRendererDX9.DeviceObject.h>
#include <d3dx9.h>

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
	ID3DXEffect*					m_shader;
	IDirect3DVertexDeclaration9*	m_vertexDeclaration;
	int32_t							m_lineCount;
	float							m_gridLength;

	Grid( EffekseerRendererDX9::RendererImplemented* renderer, ID3DXEffect* shader );
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

private:
	void DrawLine( const ::Effekseer::Vector3D& pos1, const ::Effekseer::Vector3D& pos2, const ::Effekseer::Color& col );
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRENDERER_GRID_H__