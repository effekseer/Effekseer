#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererDX11.TextureLoader.h"
#include "EffekseerRendererDX11.RendererImplemented.h"

#include "../../EffekseerRendererCommon/EffekseerRenderer.DDSTextureLoader.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.DXTK.DDSTextureLoader.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererDX11
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
TextureLoader::TextureLoader(ID3D11Device* device,
							 ID3D11DeviceContext* context,
							 ::Effekseer::FileInterface* fileInterface,
							 ::Effekseer::ColorSpaceType colorSpaceType)
	: device(device)
	, context(context)
	, m_fileInterface(fileInterface)
	, colorSpaceType_(colorSpaceType)
{
	ES_SAFE_ADDREF(device);
	ES_SAFE_ADDREF(context);

	if (fileInterface == NULL)
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

	ES_SAFE_RELEASE(device);
	ES_SAFE_RELEASE(context);
}

Effekseer::TextureData* TextureLoader::Load(const EFK_CHAR* path, ::Effekseer::TextureType textureType)
{
	std::unique_ptr<::Effekseer::FileReader> reader(m_fileInterface->OpenRead(path));

	if (reader.get() != NULL)
	{
		size_t size_texture = reader->GetLength();
		char* data_texture = new char[size_texture];
		reader->Read(data_texture, size_texture);

		Effekseer::TextureData* textureData = Load(data_texture, size_texture, textureType);
		delete[] data_texture;
		return textureData;
	}

	return nullptr;
}

Effekseer::TextureData* TextureLoader::Load(const void* data, int32_t size, Effekseer::TextureType textureType)
{
	auto size_texture = size;
	auto data_texture = (uint8_t*)data;
	ID3D11ShaderResourceView* texture = NULL;
	Effekseer::TextureData* textureData = nullptr;

	if (size_texture < 4)
	{
	}
	else if (data_texture[1] == 'P' && data_texture[2] == 'N' && data_texture[3] == 'G')
	{
		if (pngTextureLoader.Load(data_texture, size_texture, false))
		{
			DXGI_FORMAT colorFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
			if (colorSpaceType_ == ::Effekseer::ColorSpaceType::Linear && textureType == Effekseer::TextureType::Color)
				colorFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

			ID3D11Texture2D* tex = NULL;

			D3D11_TEXTURE2D_DESC TexDesc{};
			TexDesc.Width = pngTextureLoader.GetWidth();
			TexDesc.Height = pngTextureLoader.GetHeight();
			TexDesc.ArraySize = 1;
			TexDesc.Format = colorFormat;
			TexDesc.SampleDesc.Count = 1;
			TexDesc.SampleDesc.Quality = 0;
			TexDesc.Usage = D3D11_USAGE_DEFAULT;
			TexDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			TexDesc.CPUAccessFlags = 0;
			TexDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

			D3D11_SUBRESOURCE_DATA data;
			data.pSysMem = pngTextureLoader.GetData().data();
			data.SysMemPitch = TexDesc.Width * 4;
			data.SysMemSlicePitch = TexDesc.Width * TexDesc.Height * 4;

			HRESULT hr = device->CreateTexture2D(&TexDesc, nullptr, &tex);

			if (FAILED(hr))
			{
				goto Exit;
			}

			D3D11_SHADER_RESOURCE_VIEW_DESC desc{};
			desc.Format = TexDesc.Format;
			desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipLevels = -1;

			hr = device->CreateShaderResourceView(tex, &desc, &texture);
			if (FAILED(hr))
			{
				ES_SAFE_RELEASE(texture);
				goto Exit;
			}

			context->UpdateSubresource(tex, 0, 0, pngTextureLoader.GetData().data(), data.SysMemPitch, 0);

			ES_SAFE_RELEASE(tex);

			// Generate mipmap
			context->GenerateMips(texture);

			textureData = new Effekseer::TextureData();
			textureData->UserPtr = texture;
			textureData->UserID = 0;
			textureData->TextureFormat = Effekseer::TextureFormatType::ABGR8;
			textureData->Width = TexDesc.Width;
			textureData->Height = TexDesc.Height;
		}
	}
	else if (data_texture[0] == 'D' && data_texture[1] == 'D' && data_texture[2] == 'S' && data_texture[3] == ' ')
	{
		ID3D11Resource* textureR = NULL;
		EffekseerDirectX::CreateDDSTextureFromMemory(device, (uint8_t*)data_texture, size_texture, &textureR, &texture);

		ES_SAFE_RELEASE(textureR);

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
		auto texture = (ID3D11ShaderResourceView*)data->UserPtr;
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
} // namespace EffekseerRendererDX11
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

#endif // __EFFEKSEER_RENDERER_INTERNAL_LOADER__