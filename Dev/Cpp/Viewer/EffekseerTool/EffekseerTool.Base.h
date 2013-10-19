
#ifndef	__EFFEKSEERTOOL_BASE_H__
#define	__EFFEKSEERTOOL_BASE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <Effekseer.h>
#include <EffekseerRenderer/EffekseerRenderer.Renderer.h>
#include <EffekseerRenderer/EffekseerRenderer.SpriteRenderer.h>
#include <EffekseerRenderer/EffekseerRenderer.RibbonRenderer.h>

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
class Grid;
class Guide;
class Background;
}

namespace EffekseerTool
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

class Renderer;

enum eProjectionType
{
	PROJECTION_TYPE_PERSPECTIVE,
	PROJECTION_TYPE_ORTHOGRAPHIC,

	PROJECTION_TYPE_DWORD = 0x7fffffff,
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERTOOL_BASE_H__