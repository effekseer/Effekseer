
#ifndef	__EFFEKSEERRENDERER_GUIDE_H__
#define	__EFFEKSEERRENDERER_GUIDE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerTool.Renderer.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class Guide
{
private:
	
	struct Vertex
	{
		float	x;
		float	y;
	};

	EffekseerRendererDX9::RendererImplemented*			m_renderer;
	EffekseerRendererDX9::Shader*							m_shader;

	Guide( EffekseerRendererDX9::RendererImplemented* renderer, EffekseerRendererDX9::Shader* shader );
public:

	virtual ~Guide();

	static Guide* Create( EffekseerRendererDX9::RendererImplemented* renderer );


public:
	void Rendering( int32_t width, int32_t height, int32_t guide_width, int32_t guide_height );
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRENDERER_GUIDE_H__