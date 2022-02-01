﻿#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__

#include "TextureLoader.h"

namespace EffekseerRenderer
{

TextureLoader::TextureLoader(::Effekseer::Backend::GraphicsDevice* graphicsDevice,
							 ::Effekseer::FileInterfaceRef fileInterface,
							 ::Effekseer::ColorSpaceType colorSpaceType)
	: graphicsDevice_(graphicsDevice)
	, m_fileInterface(fileInterface)
	, colorSpaceType_(colorSpaceType)
{
	ES_SAFE_ADDREF(graphicsDevice_);
	if (fileInterface == nullptr)
	{
		m_fileInterface = Effekseer::MakeRefPtr<Effekseer::DefaultFileInterface>();
	}
}

TextureLoader::~TextureLoader()
{
	ES_SAFE_RELEASE(graphicsDevice_);
}

Effekseer::TextureRef TextureLoader::Load(const char16_t* path, ::Effekseer::TextureType textureType)
{
	auto reader = m_fileInterface->OpenRead(path);

	if (reader != nullptr)
	{
		auto path16 = std::u16string(path);
		auto isMipEnabled = path16.find(u"_NoMip") == std::u16string::npos;

		size_t fileSize = reader->GetLength();
		std::vector<uint8_t> fileData(fileSize);
		reader->Read(fileData.data(), fileSize);

		auto texture = Load(fileData.data(), static_cast<int32_t>(fileSize), textureType, isMipEnabled);
		return texture;
	}

	return nullptr;
}

Effekseer::TextureRef TextureLoader::Load(const void* data, int32_t size, Effekseer::TextureType textureType, bool isMipMapEnabled)
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

void TextureLoader::Unload(Effekseer::TextureRef data)
{
}

} // namespace EffekseerRenderer

#endif // __EFFEKSEER_RENDERER_INTERNAL_LOADER__