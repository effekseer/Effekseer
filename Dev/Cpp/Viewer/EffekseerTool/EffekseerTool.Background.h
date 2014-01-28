
#ifndef	__EFFEKSEERRENDERER_BACKGROUND_H__
#define	__EFFEKSEERRENDERER_BACKGROUND_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <EffekseerRenderer/EffekseerRendererDX9.Renderer.h>
#include <EffekseerRenderer/EffekseerRendererDX9.RendererImplemented.h>
#include <EffekseerRenderer/EffekseerRendererDX9.DeviceObject.h>

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class Background
	: public EffekseerRendererDX9::DeviceObject
{
private:
	
	struct Vertex
	{
		::Effekseer::Vector3D	Pos;
		::Effekseer::Vector2D	UV;
	};

	EffekseerRendererDX9::RendererImplemented*			m_renderer;
	ID3DXEffect*					m_shader;
	IDirect3DVertexDeclaration9*	m_vertexDeclaration;

	Background( EffekseerRendererDX9::RendererImplemented* renderer, ID3DXEffect* shader );
public:

	virtual ~Background();

	static Background* Create( EffekseerRendererDX9::RendererImplemented* renderer );

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