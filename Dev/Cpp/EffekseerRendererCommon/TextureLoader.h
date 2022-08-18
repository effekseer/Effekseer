#ifndef __EFFEKSEERRENDERER_TEXTURELOADER_H__
#define __EFFEKSEERRENDERER_TEXTURELOADER_H__

#include <Effekseer.h>

namespace EffekseerRenderer
{

::Effekseer::TextureLoaderRef CreateTextureLoader(::Effekseer::Backend::GraphicsDeviceRef gprahicsDevice,
												  ::Effekseer::FileInterfaceRef fileInterface = nullptr,
												  ::Effekseer::ColorSpaceType colorSpaceType = ::Effekseer::ColorSpaceType::Gamma);

} // namespace EffekseerRenderer

#endif // __EFFEKSEERRENDERER_TEXTURELOADER_H__
