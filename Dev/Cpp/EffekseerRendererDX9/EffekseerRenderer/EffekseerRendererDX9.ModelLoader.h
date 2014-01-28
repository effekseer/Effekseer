
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__

#ifndef	__EFFEKSEERRENDERER_DX9_MODELLOADER_H__
#define	__EFFEKSEERRENDERER_DX9_MODELLOADER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererDX9.RendererImplemented.h"
#include "EffekseerRendererDX9.DeviceObject.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererDX9
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class ModelLoader
	: public ::Effekseer::ModelLoader
{
private:
	Renderer*	m_renderer;
	::Effekseer::FileInterface* m_fileInterface;
	::Effekseer::DefaultFileInterface m_defaultFileInterface;

public:
	ModelLoader( Renderer* renderer, ::Effekseer::FileInterface* fileInterface );
	virtual ~ModelLoader();

public:
	void* Load( const EFK_CHAR* path );

	void Unload( void* data );
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRENDERER_DX9_MODELLOADER_H__

#endif // __EFFEKSEER_RENDERER_INTERNAL_LOADER__