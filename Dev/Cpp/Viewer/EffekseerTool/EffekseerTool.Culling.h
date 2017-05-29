
#ifndef	__EFFEKSEERRENDERER_CULLING_H__
#define	__EFFEKSEERRENDERER_CULLING_H__

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
class Culling
	: public EffekseerRendererDX9::DeviceObject
{
private:
	
	struct Vertex
	{
		::Effekseer::Vector3D	Pos;
		float	Col[4];
	};

	EffekseerRendererDX9::RendererImplemented*			m_renderer;
	EffekseerRendererDX9::Shader*						m_shader;
	int32_t							m_lineCount;

	Culling( EffekseerRendererDX9::RendererImplemented* renderer, EffekseerRendererDX9::Shader*	m_shader);
public:

	virtual ~Culling();

	static Culling* Create( EffekseerRendererDX9::RendererImplemented* renderer );

public:	// デバイス復旧用
	virtual void OnLostDevice();
	virtual void OnResetDevice();

public:
	void Rendering(bool isRightHand );

	bool IsShown;
	float Radius;
	float X;
	float Y;
	float Z;

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
#endif	// __EFFEKSEERRENDERER_CULLING_H__