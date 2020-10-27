
#ifndef __EFFEKSEERRENDERER_GL_MODELLOADER_H__
#define __EFFEKSEERRENDERER_GL_MODELLOADER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererGL.DeviceObject.h"
#include "EffekseerRendererGL.RendererImplemented.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererGL
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class ModelLoader
	: public ::Effekseer::ModelLoader
{
private:
	::Effekseer::FileInterface* m_fileInterface;
	::Effekseer::DefaultFileInterface m_defaultFileInterface;

public:
	ModelLoader(::Effekseer::FileInterface* fileInterface);
	virtual ~ModelLoader();

public:
	void* Load(const EFK_CHAR* path) override;

	void* Load(const void* data, int32_t size) override;

	void Unload(void* data) override;
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRendererGL
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_GL_MODELLOADER_H__
