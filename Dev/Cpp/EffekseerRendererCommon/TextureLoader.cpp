#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__

#include "TextureLoader.h"

namespace EffekseerRenderer
{

TextureLoader::TextureLoader(::Effekseer::Backend::GraphicsDevice* graphicsDevice,
							 ::Effekseer::FileInterface* fileInterface,
							 ::Effekseer::ColorSpaceType colorSpaceType)
	: graphicsDevice_(graphicsDevice)
	, m_fileInterface(fileInterface)
	, colorSpaceType_(colorSpaceType)
{
	ES_SAFE_ADDREF(graphicsDevice_);
	if (fileInterface == NULL)
	{
		m_fileInterface = &m_defaultFileInterface;
	}
}

TextureLoader::~TextureLoader()
{
	ES_SAFE_RELEASE(graphicsDevice_);
}

Effekseer::TextureData* TextureLoader::Load(const char16_t* path, ::Effekseer::TextureType textureType)
{
	std::unique_ptr<::Effekseer::FileReader> reader(m_fileInterface->OpenRead(path));

	if (reader.get() != NULL)
	{
		auto path16 = std::u16string(path);
		auto isMipEnabled = path16.find(u"_NoMip") == std::u16string::npos;

		size_t size_texture = reader->GetLength();
		char* data_texture = new char[size_texture];
		reader->Read(data_texture, size_texture);

		Effekseer::TextureData* textureData = Load(data_texture, static_cast<int32_t>(size_texture), textureType, isMipEnabled);
		delete[] data_texture;
		return textureData;
	}

	return nullptr;
}

Effekseer::TextureData* TextureLoader::Load(const void* data, int32_t size, Effekseer::TextureType textureType, bool isMipMapEnabled)
{
	auto size_texture = size;
	auto data_texture = (uint8_t*)data;
	Effekseer::TextureData* textureData = nullptr;

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
				param.GenerateMipmap = isMipMapEnabled;
				param.InitialData.assign(pngTextureLoader_.GetData().begin(), pngTextureLoader_.GetData().end());

				Effekseer::TextureData* textureData = nullptr;
				auto backendTexture = graphicsDevice_->CreateTexture(param);
				if (backendTexture != nullptr)
				{
					textureData = new Effekseer::TextureData();
					textureData->UserPtr = nullptr;
					textureData->UserID = 0;
					textureData->TextureFormat = Effekseer::TextureFormatType::ABGR8;
					textureData->Width = pngTextureLoader_.GetWidth();
					textureData->Height = pngTextureLoader_.GetHeight();
					textureData->HasMipmap = backendTexture->GetHasMipmap();
					textureData->TexturePtr = backendTexture;
				}

				return textureData;
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
			param.Format = ddsTextureLoader_.GetBackendTextureFormat();
			param.InitialData.assign(ddsTextureLoader_.GetTextures().at(0).Data.begin(), ddsTextureLoader_.GetTextures().at(0).Data.end());
			param.GenerateMipmap = false; // TODO : Support nomipmap

			Effekseer::TextureData* textureData = nullptr;

			auto backendTexture = graphicsDevice_->CreateTexture(param);
			if (backendTexture != nullptr)
			{
				textureData = new Effekseer::TextureData();
				textureData->UserPtr = nullptr;
				textureData->UserID = 0;
				textureData->TextureFormat = ddsTextureLoader_.GetTextureFormat();
				textureData->Width = backendTexture->GetSize()[0];
				textureData->Height = backendTexture->GetSize()[1];
				textureData->HasMipmap = backendTexture->GetHasMipmap();
				textureData->TexturePtr = backendTexture;
			}

			return textureData;
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
				param.GenerateMipmap = isMipMapEnabled;
				param.InitialData.assign(tgaTextureLoader_.GetData().begin(), tgaTextureLoader_.GetData().end());

				Effekseer::TextureData* textureData = nullptr;

				auto backendTexture = graphicsDevice_->CreateTexture(param);
				if (backendTexture != nullptr)
				{
					textureData = new Effekseer::TextureData();
					textureData->UserPtr = nullptr;
					textureData->UserID = 0;
					textureData->TextureFormat = Effekseer::TextureFormatType::ABGR8;
					textureData->Width = tgaTextureLoader_.GetWidth();
					textureData->Height = tgaTextureLoader_.GetHeight();
					textureData->TexturePtr = backendTexture;
					textureData->HasMipmap = backendTexture->GetHasMipmap();
				}

				return textureData;
			}
		}
	}

Exit:;
	return textureData;
}

void TextureLoader::Unload(Effekseer::TextureData* data)
{
	if (data != nullptr && data->TexturePtr != nullptr)
	{
		ES_SAFE_RELEASE(data->TexturePtr);
	}

	if (data != nullptr)
	{
		delete data;
	}
}

} // namespace EffekseerRenderer

#endif // __EFFEKSEER_RENDERER_INTERNAL_LOADER__