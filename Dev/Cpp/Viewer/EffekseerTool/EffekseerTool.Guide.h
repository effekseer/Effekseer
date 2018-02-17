
#ifndef	__EFFEKSEERRENDERER_GUIDE_H__
#define	__EFFEKSEERRENDERER_GUIDE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerTool.Renderer.h"
#include "../Graphics/Platform/DX9/efk.ImageRendererDX9.h"

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
	
	EffekseerRendererDX9::RendererImplemented*			m_renderer;
	efk::ImageRendererDX9*								imageRenderer = nullptr;

	Guide( EffekseerRendererDX9::RendererImplemented* renderer);
public:

	virtual ~Guide();

	static Guide* Create( EffekseerRendererDX9::RendererImplemented* renderer );

public:	// デバイス復旧用
	virtual void OnLostDevice();
	virtual void OnResetDevice();

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