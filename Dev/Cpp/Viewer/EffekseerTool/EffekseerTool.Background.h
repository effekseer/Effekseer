
#ifndef	__EFFEKSEERRENDERER_BACKGROUND_H__
#define	__EFFEKSEERRENDERER_BACKGROUND_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <EffekseerRenderer/EffekseerRenderer.Renderer.h>
#include <EffekseerRenderer/EffekseerRenderer.RendererImplemented.h>
#include <EffekseerRenderer/EffekseerRenderer.DeviceObject.h>

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class Background
	: public DeviceObject
{
private:
	
	struct Vertex
	{
		::Effekseer::Vector3D	Pos;
		::Effekseer::Vector2D	UV;
	};

	RendererImplemented*			m_renderer;
	ID3DXEffect*					m_shader;
	IDirect3DVertexDeclaration9*	m_vertexDeclaration;

	Background( RendererImplemented* renderer, ID3DXEffect* shader );
public:

	virtual ~Background();

	static Background* Create( RendererImplemented* renderer );

public:	// デバイス復旧用
	virtual void OnLostDevice();
	virtual void OnResetDevice();

public:
	void Rendering( IDirect3DTexture9* texture );
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRENDERER_BACKGROUND_H__