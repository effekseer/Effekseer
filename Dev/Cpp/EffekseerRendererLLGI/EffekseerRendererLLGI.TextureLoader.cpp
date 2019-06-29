#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__

#include "EffekseerRendererLLGI.TextureLoader.h"
#include "EffekseerRendererLLGI.RendererImplemented.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../3rdParty/stb/stb_image.h"

namespace EffekseerRendererLLGI
{

TextureLoader::TextureLoader(LLGI::Graphics* graphics, ::Effekseer::FileInterface* fileInterface)
	: m_fileInterface(fileInterface), graphics(graphics)
{
	ES_SAFE_ADDREF(graphics);

	if (fileInterface == nullptr)
	{
		m_fileInterface = &m_defaultFileInterface;
	}
}

TextureLoader::~TextureLoader() { ES_SAFE_RELEASE(graphics); }

Effekseer::TextureData* TextureLoader::Load(const EFK_CHAR* path, ::Effekseer::TextureType textureType)
{
	std::auto_ptr<::Effekseer::FileReader> reader(m_fileInterface->OpenRead(path));

	if (reader.get() != NULL)
	{
		Effekseer::TextureData* textureData = nullptr;

		size_t size_texture = reader->GetLength();
		char* data_texture = new char[size_texture];
		reader->Read(data_texture, size_texture);

		unsigned char* pixels;
		int width;
		int height;
		int bpp;

		pixels = (uint8_t*)stbi_load_from_memory((stbi_uc const*)data_texture, size_texture, &width, &height, &bpp, 0);

		if (width > 0)
		{
			auto texture = graphics->CreateTexture(LLGI::Vec2I(width, height), false, false);
			auto buf = texture->Lock();

			if (bpp == 4)
			{
				memcpy(buf, pixels, width * height * 4);
			}
			else if (bpp == 2)
			{
				// Gray+Alpha
				for (int h = 0; h < height; h++)
				{
					for (int w = 0; w < width; w++)
					{
						((uint8_t*)buf)[(w + h * width) * 4 + 0] = pixels[(w + h * width) * 2 + 0];
						((uint8_t*)buf)[(w + h * width) * 4 + 1] = pixels[(w + h * width) * 2 + 0];
						((uint8_t*)buf)[(w + h * width) * 4 + 2] = pixels[(w + h * width) * 2 + 0];
						((uint8_t*)buf)[(w + h * width) * 4 + 3] = pixels[(w + h * width) * 2 + 1];
					}
				}
			}
			else if (bpp == 1)
			{
				// Gray
				for (int h = 0; h < height; h++)
				{
					for (int w = 0; w < width; w++)
					{
						((uint8_t*)buf)[(w + h * width) * 4 + 0] = pixels[(w + h * width) * 2 + 0];
						((uint8_t*)buf)[(w + h * width) * 4 + 1] = pixels[(w + h * width) * 2 + 0];
						((uint8_t*)buf)[(w + h * width) * 4 + 2] = pixels[(w + h * width) * 2 + 0];
						((uint8_t*)buf)[(w + h * width) * 4 + 3] = 255;
					}
				}
			}
			else
			{
				for (int h = 0; h < height; h++)
				{
					for (int w = 0; w < width; w++)
					{
						((uint8_t*)buf)[(w + h * width) * 4 + 0] = pixels[(w + h * width) * 3 + 0];
						((uint8_t*)buf)[(w + h * width) * 4 + 1] = pixels[(w + h * width) * 3 + 1];
						((uint8_t*)buf)[(w + h * width) * 4 + 2] = pixels[(w + h * width) * 3 + 2];
						((uint8_t*)buf)[(w + h * width) * 4 + 3] = 255;
					}
				}
			}

			texture->Unlock();

			textureData = new Effekseer::TextureData();
			textureData->UserPtr = texture;
			textureData->UserID = 0;
			textureData->TextureFormat = Effekseer::TextureFormatType::ABGR8;
			textureData->Width = width;
			textureData->Height = height;
		}

		delete[] data_texture;
		stbi_image_free(pixels);
		return textureData;
	}

	return nullptr;
}

void TextureLoader::Unload(Effekseer::TextureData* data)
{
	if (data != nullptr && data->UserPtr != nullptr)
	{
		auto texture = (LLGI::Texture*)data->UserPtr;
		texture->Release();
	}

	if (data != nullptr)
	{
		delete data;
	}
}

} // namespace EffekseerRendererLLGI

#endif // __EFFEKSEER_RENDERER_INTERNAL_LOADER__