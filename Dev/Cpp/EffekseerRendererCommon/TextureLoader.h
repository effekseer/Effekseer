#ifndef __EFFEKSEERRENDERER_TEXTURELOADER_H__
#define __EFFEKSEERRENDERER_TEXTURELOADER_H__

#include <Effekseer.h>

namespace EffekseerRenderer
{

::Effekseer::TextureLoaderRef CreateTextureLoader(::Effekseer::Backend::GraphicsDeviceRef gprahicsDevice,
												  ::Effekseer::FileInterfaceRef fileInterface = nullptr,
												  ::Effekseer::ColorSpaceType colorSpaceType = ::Effekseer::ColorSpaceType::Gamma);

class TextureLoader : public ::Effekseer::TextureLoader
{
	class Impl;

private:
	std::unique_ptr<Impl> impl_;

public:
	TextureLoader(::Effekseer::Backend::GraphicsDeviceRef gprahicsDevice,
				  ::Effekseer::FileInterfaceRef fileInterface = nullptr,
				  ::Effekseer::ColorSpaceType colorSpaceType = ::Effekseer::ColorSpaceType::Gamma);
	virtual ~TextureLoader() override = default;

public:
	Effekseer::TextureRef Load(const char16_t* path, ::Effekseer::TextureType textureType) override;

	Effekseer::TextureRef Load(const void* data, int32_t size, Effekseer::TextureType textureType, bool isMipMapEnabled) override;
};

} // namespace EffekseerRenderer

#endif // __EFFEKSEERRENDERER_TEXTURELOADER_H__
