
#ifndef __EFFEKSEERRENDERER_LLGI_MATERIALLOADER_H__
#define __EFFEKSEERRENDERER_LLGI_MATERIALLOADER_H__

#include "EffekseerRendererLLGI.DeviceObject.h"
#include "EffekseerRendererLLGI.RendererImplemented.h"
#include "../Effekseer/Effekseer/Material/Effekseer.MaterialCompiler.h"

#include <Effekseer.h>
#include <memory>

namespace Effekseer
{
class Material;
class CompiledMaterialBinary;
} // namespace Effekseer

namespace EffekseerRendererLLGI
{

class MaterialLoader : public ::Effekseer::MaterialLoader
{
private:
	Renderer* renderer_ = nullptr;
	::Effekseer::FileInterface* fileInterface_ = nullptr;
	::Effekseer::CompiledMaterialPlatformType platformType_;
	::Effekseer::MaterialCompiler* materialCompiler_ = nullptr;
	::Effekseer::DefaultFileInterface defaultFileInterface_;

	::Effekseer::MaterialData* LoadAcutually(::Effekseer::Material& material, ::Effekseer::CompiledMaterialBinary* binary);

public:
	MaterialLoader(Renderer* renderer,
				   ::Effekseer::FileInterface* fileInterface,
				   ::Effekseer::CompiledMaterialPlatformType platformType,
				   ::Effekseer::MaterialCompiler* materialCompiler);
	virtual ~MaterialLoader();

	::Effekseer::MaterialData* Load(const EFK_CHAR* path) override;

	::Effekseer::MaterialData* Load(const void* data, int32_t size, Effekseer::MaterialFileType fileType) override;

	void Unload(::Effekseer::MaterialData* data) override;
};

}

#endif // __EFFEKSEERRENDERER_LLGI_MATERIALLOADER_H__
