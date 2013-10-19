
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

	RendererImplemented*			m_renderer;
	Shader*							m_shader;

	Guide( RendererImplemented* renderer, Shader* shader );
public:

	virtual ~Guide();

	static Guide* Create( RendererImplemented* renderer );


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