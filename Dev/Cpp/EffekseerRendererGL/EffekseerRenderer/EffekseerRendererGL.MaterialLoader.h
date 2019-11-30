
#ifndef __EFFEKSEERRENDERER_GL_MATERIALLOADER_H__
#define __EFFEKSEERRENDERER_GL_MATERIALLOADER_H__

#include "EffekseerRendererGL.RendererImplemented.h"

namespace Effekseer
{
class Material;
class CompiledMaterialBinary;
} // namespace Effekseer

namespace EffekseerRendererGL
{

class DeviceObjectCollection;

class MaterialLoader : public ::Effekseer::MaterialLoader
{
private:
	OpenGLDeviceType deviceType_;
	Renderer* renderer_ = nullptr;
	DeviceObjectCollection* deviceObjectCollection_ = nullptr;
	bool canLoadFromCache_ = false;

	::Effekseer::FileInterface* fileInterface_ = nullptr;
	::Effekseer::DefaultFileInterface defaultFileInterface_;

	::Effekseer::MaterialData* LoadAcutually(::Effekseer::Material& material, ::Effekseer::CompiledMaterialBinary* binary);

public:
	MaterialLoader(OpenGLDeviceType deviceType,
				   Renderer* renderer,
				   DeviceObjectCollection* deviceObjectCollection,
				   ::Effekseer::FileInterface* fileInterface,
				   bool canLoadFromCache = true);
	virtual ~MaterialLoader();

	::Effekseer::MaterialData* Load(const EFK_CHAR* path) override;

	::Effekseer::MaterialData* Load(const void* data, int32_t size, Effekseer::MaterialFileType fileType) override;

	void Unload(::Effekseer::MaterialData* data) override;
};

} // namespace EffekseerRendererGL

#endif // __EFFEKSEERRENDERER_GL_MODELLOADER_H__
