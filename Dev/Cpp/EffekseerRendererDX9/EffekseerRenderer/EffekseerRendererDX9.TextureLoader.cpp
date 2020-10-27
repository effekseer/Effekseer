
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererDX9.TextureLoader.h"
#include "EffekseerRendererDX9.RendererImplemented.h"
#include <memory>

#include "../../EffekseerRendererCommon/EffekseerRenderer.DDSTextureLoader.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.DXTK.DDSTextureLoader.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.PngTextureLoader.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererDX9
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
TextureLoader::TextureLoader(RendererImplemented* renderer, ::Effekseer::FileInterface* fileInterface)
	: renderer_(renderer)
	, m_fileInterface(fileInterface)
{
	ES_SAFE_ADDREF(renderer_);

	if (m_fileInterface == NULL)
	{
		m_fileInterface = &m_defaultFileInterface;
	}

#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
	pngTextureLoader.Initialize();
#endif
}

TextureLoader::TextureLoader(LPDIRECT3DDEVICE9 device, ::Effekseer::FileInterface* fileInterface)
	: device_(device)
	, m_fileInterface(fileInterface)
{
	ES_SAFE_ADDREF(device);

	if (m_fileInterface == NULL)
	{
		m_fileInterface = &m_defaultFileInterface;
	}

#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
	pngTextureLoader.Initialize();
#endif
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
TextureLoader::~TextureLoader()
{
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
	pngTextureLoader.Finalize();
#endif

	ES_SAFE_RELEASE(device_);
	ES_SAFE_RELEASE(renderer_);
}

Effekseer::TextureData* TextureLoader::Load(const EFK_CHAR* path, ::Effekseer::TextureType textureType)
{
	std::unique_ptr<::Effekseer::FileReader> reader(m_fileInterface->OpenRead(path));

	if (reader.get() != nullptr)
	{
		size_t size_texture = reader->GetLength();
		char* data_texture = new char[size_texture];
		reader->Read(data_texture, size_texture);

		Effekseer::TextureData* textureData = Load(data_texture, static_cast<int32_t>(size_texture), textureType);
		delete[] data_texture;
		return textureData;
	}

	return nullptr;
}

Effekseer::TextureData* TextureLoader::Load(const void* data, int32_t size, Effekseer::TextureType textureType)
{
	// get device
	LPDIRECT3DDEVICE9 device = nullptr;
	if (device_ != nullptr)
	{
		device = device_;
	}
	else if (renderer_ != nullptr)
	{
		device = renderer_->GetDevice();
	}
	else
	{
		return nullptr;
	}

	auto size_texture = size;
	auto data_texture = (uint8_t*)data;

	IDirect3DTexture9* texture = nullptr;
	Effekseer::TextureData* textureData = nullptr;

	if (size_texture < 4)
	{
	}
	else if (data_texture[1] == 'P' && data_texture[2] == 'N' && data_texture[3] == 'G')
	{
		if (pngTextureLoader.Load(data_texture, size_texture, false))
		{
			HRESULT hr;
			int32_t width = pngTextureLoader.GetWidth();
			int32_t height = pngTextureLoader.GetHeight();
			int32_t mipMapCount = 1;
			hr =
				device->CreateTexture(width, height, mipMapCount, D3DUSAGE_AUTOGENMIPMAP, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &texture, NULL);

			if (FAILED(hr))
			{
				pngTextureLoader.Unload();
				goto Exit;
			}

			LPDIRECT3DTEXTURE9 tempTexture = NULL;
			hr = device->CreateTexture(width, height, mipMapCount, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &tempTexture, NULL);

			if (FAILED(hr))
			{
				pngTextureLoader.Unload();
				goto Exit;
			}

			uint8_t* srcBits = (uint8_t*)pngTextureLoader.GetData().data();
			D3DLOCKED_RECT locked;
			if (SUCCEEDED(tempTexture->LockRect(0, &locked, NULL, 0)))
			{
				uint8_t* destBits = (uint8_t*)locked.pBits;

				for (int32_t h = 0; h < height; h++)
				{
					memcpy(destBits, srcBits, width * 4);

					// RGB入れ替え
					for (int32_t w = 0; w < width; w++)
					{
						std::swap(destBits[w * 4 + 0], destBits[w * 4 + 2]);
					}

					destBits += locked.Pitch;
					srcBits += (width * 4);
				}

				tempTexture->UnlockRect(0);
			}

			hr = device->UpdateTexture(tempTexture, texture);
			ES_SAFE_RELEASE(tempTexture);

			pngTextureLoader.Unload();

			textureData = new Effekseer::TextureData();
			textureData->UserPtr = texture;
			textureData->UserID = 0;
			textureData->TextureFormat = Effekseer::TextureFormatType::ABGR8;
			textureData->Width = width;
			textureData->Height = height;
		}
	}
	else if (data_texture[0] == 'D' && data_texture[1] == 'D' && data_texture[2] == 'S' && data_texture[3] == ' ')
	{
		EffekseerDirectX::CreateDDSTextureFromMemory(device, (uint8_t*)data_texture, size_texture, texture);

		// To get texture size, use loader
		ddsTextureLoader.Load(data_texture, size_texture);

		textureData = new Effekseer::TextureData();
		textureData->UserPtr = texture;
		textureData->UserID = 0;
		textureData->TextureFormat = ddsTextureLoader.GetTextureFormat();
		textureData->Width = ddsTextureLoader.GetWidth();
		textureData->Height = ddsTextureLoader.GetHeight();
	}

Exit:;
	return textureData;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void TextureLoader::Unload(Effekseer::TextureData* data)
{
	if (data != nullptr && data->UserPtr != nullptr)
	{
		IDirect3DTexture9* texture = (IDirect3DTexture9*)data->UserPtr;
		texture->Release();
	}

	if (data != nullptr)
	{
		delete data;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRendererDX9
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

#endif // __EFFEKSEER_RENDERER_INTERNAL_LOADER__