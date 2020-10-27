#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__

#include "EffekseerRendererLLGI.TextureLoader.h"
#include "EffekseerRendererLLGI.RendererImplemented.h"

namespace EffekseerRendererLLGI
{

TextureLoader::TextureLoader(GraphicsDevice* graphicsDevice, ::Effekseer::FileInterface* fileInterface)
	: m_fileInterface(fileInterface)
	, graphicsDevice_(graphicsDevice)
{
	ES_SAFE_ADDREF(graphicsDevice);

	if (fileInterface == nullptr)
	{
		m_fileInterface = &m_defaultFileInterface;
	}

	pngTextureLoader_.Initialize();
}

TextureLoader::~TextureLoader()
{
	ES_SAFE_RELEASE(graphicsDevice_);
	pngTextureLoader_.Finalize();
}

Effekseer::TextureData* TextureLoader::Load(const EFK_CHAR* path, ::Effekseer::TextureType textureType)
{
	std::unique_ptr<::Effekseer::FileReader> reader(m_fileInterface->OpenRead(path));

	if (reader.get() != NULL)
	{
		Effekseer::TextureData* textureData = nullptr;

		size_t size_texture = reader->GetLength();
		char* data_texture = new char[size_texture];
		reader->Read(data_texture, size_texture);

		if (pngTextureLoader_.Load(data_texture, size_texture, false))
		{
			LLGI::TextureInitializationParameter texParam;
			texParam.Size = LLGI::Vec2I(pngTextureLoader_.GetWidth(), pngTextureLoader_.GetHeight());
			auto texture = graphicsDevice_->GetGraphics()->CreateTexture(texParam);
			auto buf = texture->Lock();

			memcpy(buf, pngTextureLoader_.GetData().data(), pngTextureLoader_.GetWidth() * pngTextureLoader_.GetHeight() * 4);

			texture->Unlock();

			textureData = new Effekseer::TextureData();
			textureData->UserPtr = texture;
			textureData->UserID = 0;
			textureData->TextureFormat = Effekseer::TextureFormatType::ABGR8;
			textureData->Width = pngTextureLoader_.GetWidth();
			textureData->Height = pngTextureLoader_.GetHeight();
		}

		delete[] data_texture;
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