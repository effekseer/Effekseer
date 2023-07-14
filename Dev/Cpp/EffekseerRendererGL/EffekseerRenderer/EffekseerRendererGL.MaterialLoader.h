
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

void StoreVertexUniform(const ::Effekseer::MaterialFile& materialFile, const EffekseerRenderer::MaterialShaderParameterGenerator& generator, Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement>& uniformLayout);

void StoreModelVertexUniform(const ::Effekseer::MaterialFile& materialFile, const EffekseerRenderer::MaterialShaderParameterGenerator& generator, Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement>& uniformLayout, bool instancing);

void StorePixelUniform(const ::Effekseer::MaterialFile& materialFile, const EffekseerRenderer::MaterialShaderParameterGenerator& generator, Effekseer::CustomVector<Effekseer::Backend::UniformLayoutElement>& uniformLayout, int shaderType);

Effekseer::CustomVector<Effekseer::CustomString<char>> StoreTextureLocations(const ::Effekseer::MaterialFile& materialFile);

class MaterialLoader : public ::Effekseer::MaterialLoader
{
private:
	Backend::GraphicsDeviceRef graphicsDevice_ = nullptr;
	bool canLoadFromCache_ = false;

	::Effekseer::FileInterfaceRef fileInterface_ = nullptr;
	::Effekseer::DefaultFileInterface defaultFileInterface_;

	::Effekseer::MaterialRef LoadAcutually(::Effekseer::MaterialFile& materialFile, ::Effekseer::CompiledMaterialBinary* binary);

public:
	MaterialLoader(Backend::GraphicsDeviceRef graphicsDevice, ::Effekseer::FileInterfaceRef fileInterface, bool canLoadFromCache = true);
	virtual ~MaterialLoader();

	::Effekseer::MaterialRef Load(const char16_t* path) override;

	::Effekseer::MaterialRef Load(const void* data, int32_t size, Effekseer::MaterialFileType fileType) override;

	void Unload(::Effekseer::MaterialRef data) override;
};

} // namespace EffekseerRendererGL

#endif // __EFFEKSEERRENDERER_GL_MODELLOADER_H__
