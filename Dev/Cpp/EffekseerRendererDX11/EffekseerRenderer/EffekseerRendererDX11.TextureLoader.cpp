#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererDX11.RendererImplemented.h"
#include "EffekseerRendererDX11.TextureLoader.h"

#include "../../EffekseerRendererCommon/EffekseerRenderer.DXTK.DDSTextureLoader.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.PngTextureLoader.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.DDSTextureLoader.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererDX11
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
TextureLoader::TextureLoader(ID3D11Device* device, ::Effekseer::FileInterface* fileInterface )
	: m_fileInterface	(fileInterface)
	, device			(device)
{
	ES_SAFE_ADDREF(device);

	if( fileInterface == NULL )
	{
		m_fileInterface = &m_defaultFileInterface;
	}

#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
	EffekseerRenderer::PngTextureLoader::Initialize();
#endif
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
TextureLoader::~TextureLoader()
{
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
	EffekseerRenderer::PngTextureLoader::Finalize();
#endif

	ES_SAFE_RELEASE(device);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Effekseer::TextureData* TextureLoader::Load(const EFK_CHAR* path, ::Effekseer::TextureType textureType)
{
	std::auto_ptr<::Effekseer::FileReader> 
		reader( m_fileInterface->OpenRead( path ) );
	
	if( reader.get() != NULL )
	{
		ID3D11ShaderResourceView* texture = NULL;
		Effekseer::TextureData* textureData = nullptr;

		size_t size_texture = reader->GetLength();
		char* data_texture = new char[size_texture];
		reader->Read( data_texture, size_texture );

		if( size_texture < 4 )
		{
		}
		else if(data_texture[1] == 'P' &&
			data_texture[2] == 'N' &&
			data_texture[3] == 'G')
		{
			if(::EffekseerRenderer::PngTextureLoader::Load(data_texture, size_texture, false))
			{
				ID3D11Texture2D* tex = NULL;

				D3D11_TEXTURE2D_DESC TexDesc;
				TexDesc.Width = ::EffekseerRenderer::PngTextureLoader::GetWidth();
				TexDesc.Height = ::EffekseerRenderer::PngTextureLoader::GetHeight();
				TexDesc.MipLevels = 1;
				TexDesc.ArraySize = 1;
				TexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				TexDesc.SampleDesc.Count = 1;
				TexDesc.SampleDesc.Quality = 0;
				TexDesc.Usage = D3D11_USAGE_DEFAULT;
				TexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
				TexDesc.CPUAccessFlags = 0;
				TexDesc.MiscFlags = 0;

				D3D11_SUBRESOURCE_DATA data;
				data.pSysMem = ::EffekseerRenderer::PngTextureLoader::GetData().data();
				data.SysMemPitch = TexDesc.Width * 4;
				data.SysMemSlicePitch = TexDesc.Width * TexDesc.Height * 4;

				HRESULT hr = device->CreateTexture2D(&TexDesc, &data, &tex);

				if (FAILED(hr))
				{
					goto Exit;
				}
			
				D3D11_SHADER_RESOURCE_VIEW_DESC desc;
				ZeroMemory(&desc, sizeof(desc));
				desc.Format = TexDesc.Format;
				desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				desc.Texture2D.MostDetailedMip = 0;
				desc.Texture2D.MipLevels = TexDesc.MipLevels;

				hr = device->CreateShaderResourceView(tex, &desc, &texture);
				if (FAILED(hr))
				{
					ES_SAFE_RELEASE(texture);
					goto Exit;
				}

				ES_SAFE_RELEASE(tex);

				textureData = new Effekseer::TextureData();
				textureData->UserPtr = texture;
				textureData->UserID = 0;
				textureData->TextureFormat = Effekseer::TextureFormatType::ABGR8;
				textureData->Width = TexDesc.Width;
				textureData->Height = TexDesc.Height;
			}
		}
		else if( data_texture[0] == 'D' &&
			data_texture[1] == 'D' &&
			data_texture[2] == 'S' &&
			data_texture[3] == ' ')
		{
			ID3D11Resource* textureR = NULL;
			EffekseerDirectX::CreateDDSTextureFromMemory(
				device,
				(uint8_t*)data_texture,
				size_texture,
				&textureR,
				&texture );

			ES_SAFE_RELEASE(textureR);

			// To get texture size, use loader
			EffekseerRenderer::DDSTextureLoader::Load(data_texture, size_texture);

			textureData = new Effekseer::TextureData();
			textureData->UserPtr = texture;
			textureData->UserID = 0;
			textureData->TextureFormat = EffekseerRenderer::DDSTextureLoader::GetTextureFormat();
			textureData->Width = EffekseerRenderer::DDSTextureLoader::GetWidth();
			textureData->Height = EffekseerRenderer::DDSTextureLoader::GetHeight();
		}

	Exit:;
		delete[] data_texture;
		return textureData;
	}

	return nullptr;
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
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

#endif // __EFFEKSEER_RENDERER_INTERNAL_LOADER__