
#ifndef __EFFEKSEERRENDERER_GL_MODELLOADER_H__
#define __EFFEKSEERRENDERER_GL_MODELLOADER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererGL.DeviceObject.h"
#include "EffekseerRendererGL.RendererImplemented.h"
#include "GraphicsDevice.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererGL
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class ModelLoader : public ::Effekseer::ModelLoader
{
private:
	::Effekseer::FileInterface* m_fileInterface;
	::Effekseer::DefaultFileInterface m_defaultFileInterface;
	Backend::GraphicsDevice* graphicsDevice_ = nullptr;

public:
	ModelLoader(::Effekseer::FileInterface* fileInterface, OpenGLDeviceType deviceType);
	virtual ~ModelLoader();

public:
	Effekseer::Model* Load(const char16_t* path) override;

	Effekseer::Model* Load(const void* data, int32_t size) override;

	void Unload(Effekseer::Model* data) override;
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRendererGL
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_GL_MODELLOADER_H__
