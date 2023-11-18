#include "TextureLoader.h"

#ifndef __DISABLED_DEFAULT_TEXTURE_LOADER__
#include "EffekseerRenderer.DDSTextureLoader.h"
#include "EffekseerRenderer.PngTextureLoader.h"
#include "EffekseerRenderer.TGATextureLoader.h"
#endif

namespace EffekseerRenderer
{

#ifndef __DISABLED_DEFAULT_TEXTURE_LOADER__
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
#endif

::Effekseer::TextureLoaderRef CreateTextureLoader(::Effekseer::Backend::GraphicsDeviceRef gprahicsDevice,
												  ::Effekseer::FileInterfaceRef fileInterface,
												  ::Effekseer::ColorSpaceType colorSpaceType)
{
#ifdef __DISABLED_DEFAULT_TEXTURE_LOADER__
	return nullptr;
#else
	return ::Effekseer::MakeRefPtr<TextureLoader>(gprahicsDevice, fileInterface, colorSpaceType);
#endif
}

#ifndef __DISABLED_DEFAULT_TEXTURE_LOADER__
class TextureLoader::Impl
{
public:
	Impl(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice,
		 ::Effekseer::FileInterfaceRef fileInterface,
		 ::Effekseer::ColorSpaceType colorSpaceType)
		: graphicsDevice_(graphicsDevice)
		, fileInterface_(fileInterface)
		, colorSpaceType_(colorSpaceType)
	{
		if (fileInterface == nullptr)
		{
			fileInterface_ = Effekseer::MakeRefPtr<Effekseer::DefaultFileInterface>();
		}
	}

	Effekseer::TextureRef Load(const char16_t* path, ::Effekseer::TextureType textureType)
	{
		auto reader = fileInterface_->OpenRead(path);

		if (reader != nullptr)
		{
			auto path16 = std::u16string(path);
			auto isMipEnabled = Effekseer::TextureLoaderHelper::GetIsMipmapEnabled(path16);

			size_t fileSize = reader->GetLength();
			std::vector<uint8_t> fileData(fileSize);
			reader->Read(fileData.data(), fileSize);

			auto texture = Load(fileData.data(), static_cast<int32_t>(fileSize), textureType, isMipEnabled);
			return texture;
		}

		return nullptr;
	}

	Effekseer::TextureRef Load(const void* data, int32_t size, Effekseer::TextureType textureType, bool isMipMapEnabled)
	{
		auto size_texture = size;
		auto data_texture = (uint8_t*)data;

		if (size_texture < 4)
		{
		}
		else if (data_texture[1] == 'P' && data_texture[2] == 'N' && data_texture[3] == 'G')
		{
			if (pngTextureLoader_.Load(data_texture, size_texture, false))
			{
				// Newbackend
				{
					::Effekseer::Backend::TextureFormatType format;
					if (colorSpaceType_ == ::Effekseer::ColorSpaceType::Linear && textureType == Effekseer::TextureType::Color)
					{
						format = ::Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM_SRGB;
					}
					else
					{
						format = ::Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM;
					}

					::Effekseer::Backend::TextureParameter param;
					param.Size[0] = pngTextureLoader_.GetWidth();
					param.Size[1] = pngTextureLoader_.GetHeight();
					param.Format = format;
					param.MipLevelCount = isMipMapEnabled ? 0 : 1;
					param.Dimension = 2;

					Effekseer::CustomVector<uint8_t> initialData;
					initialData.assign(pngTextureLoader_.GetData().begin(), pngTextureLoader_.GetData().end());

					auto texture = ::Effekseer::MakeRefPtr<::Effekseer::Texture>();
					texture->SetBackend(graphicsDevice_->CreateTexture(param, initialData));
					return texture;
				}
			}
		}
		else if (data_texture[0] == 'D' && data_texture[1] == 'D' && data_texture[2] == 'S' && data_texture[3] == ' ')
		{
			// Newbackend
			if (ddsTextureLoader_.Load(data_texture, size_texture))
			{
				::Effekseer::Backend::TextureFormatType format;
				if (colorSpaceType_ == ::Effekseer::ColorSpaceType::Linear && textureType == Effekseer::TextureType::Color)
				{
					format = ::Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM_SRGB;
				}
				else
				{
					format = ::Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM;
				}

				::Effekseer::Backend::TextureParameter param;
				param.Size[0] = ddsTextureLoader_.GetTextures().at(0).Width;
				param.Size[1] = ddsTextureLoader_.GetTextures().at(0).Height;
				param.Dimension = 2;
				param.Format = ddsTextureLoader_.GetBackendTextureFormat();

				Effekseer::CustomVector<uint8_t> initialData;
				initialData.assign(ddsTextureLoader_.GetTextures().at(0).Data.begin(), ddsTextureLoader_.GetTextures().at(0).Data.end());
				param.MipLevelCount = 1; // TODO : Support nomipmap

				auto texture = ::Effekseer::MakeRefPtr<::Effekseer::Texture>();
				texture->SetBackend(graphicsDevice_->CreateTexture(param, initialData));
				return texture;
			}
		}
		else
		{
			if (tgaTextureLoader_.Load(data_texture, size_texture) == true)
			{
				// Newbackend
				{
					::Effekseer::Backend::TextureFormatType format;
					if (colorSpaceType_ == ::Effekseer::ColorSpaceType::Linear && textureType == Effekseer::TextureType::Color)
					{
						format = ::Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM_SRGB;
					}
					else
					{
						format = ::Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM;
					}

					::Effekseer::Backend::TextureParameter param;
					param.Size[0] = tgaTextureLoader_.GetWidth();
					param.Size[1] = tgaTextureLoader_.GetHeight();
					param.Format = format;
					param.MipLevelCount = isMipMapEnabled ? 0 : 1;
					param.Dimension = 2;
					Effekseer::CustomVector<uint8_t> initialData;
					initialData.assign(tgaTextureLoader_.GetData().begin(), tgaTextureLoader_.GetData().end());

					auto texture = ::Effekseer::MakeRefPtr<::Effekseer::Texture>();
					texture->SetBackend(graphicsDevice_->CreateTexture(param, initialData));
					return texture;
				}
			}
		}

		return nullptr;
	}

private:
	::Effekseer::Backend::GraphicsDeviceRef graphicsDevice_;
	::Effekseer::FileInterfaceRef fileInterface_;
	::Effekseer::ColorSpaceType colorSpaceType_;
	::EffekseerRenderer::PngTextureLoader pngTextureLoader_;
	::EffekseerRenderer::DDSTextureLoader ddsTextureLoader_;
	::EffekseerRenderer::TGATextureLoader tgaTextureLoader_;
};

TextureLoader::TextureLoader(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice,
							 ::Effekseer::FileInterfaceRef fileInterface,
							 ::Effekseer::ColorSpaceType colorSpaceType)
{
	impl_ = std::make_unique<Impl>(graphicsDevice, fileInterface, colorSpaceType);
}

Effekseer::TextureRef TextureLoader::Load(const char16_t* path, ::Effekseer::TextureType textureType)
{
	return impl_->Load(path, textureType);
}

Effekseer::TextureRef TextureLoader::Load(const void* data, int32_t size, Effekseer::TextureType textureType, bool isMipMapEnabled)
{
	return impl_->Load(data, size, textureType, isMipMapEnabled);
}

#endif

} // namespace EffekseerRenderer
