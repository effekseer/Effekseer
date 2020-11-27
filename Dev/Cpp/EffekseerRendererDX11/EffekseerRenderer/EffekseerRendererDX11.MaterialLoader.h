
#ifndef __EFFEKSEERRENDERER_DX11_MATERIALLOADER_H__
#define __EFFEKSEERRENDERER_DX11_MATERIALLOADER_H__

#include "EffekseerRendererDX11.DeviceObject.h"
#include "EffekseerRendererDX11.RendererImplemented.h"
#include <memory>

namespace Effekseer
{
class Material;
class CompiledMaterialBinary;
} // namespace Effekseer

namespace EffekseerRendererDX11
{

class MaterialLoader : public ::Effekseer::MaterialLoader
{
private:
	RendererImplementedRef renderer_;
	::Effekseer::FileInterface* fileInterface_ = nullptr;
	::Effekseer::DefaultFileInterface defaultFileInterface_;

	::Effekseer::MaterialData* LoadAcutually(::Effekseer::Material& material, ::Effekseer::CompiledMaterialBinary* binary);

public:
	MaterialLoader(const RendererImplementedRef& renderer, ::Effekseer::FileInterface* fileInterface);
	virtual ~MaterialLoader();

	::Effekseer::MaterialData* Load(const char16_t* path) override;

	::Effekseer::MaterialData* Load(const void* data, int32_t size, Effekseer::MaterialFileType fileType) override;

	void Unload(::Effekseer::MaterialData* data) override;
};

} // namespace EffekseerRendererDX11

#endif // __EFFEKSEERRENDERER_GL_MODELLOADER_H__
