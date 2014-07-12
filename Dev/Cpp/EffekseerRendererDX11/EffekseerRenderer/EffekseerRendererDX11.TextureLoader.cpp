#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererDX11.RendererImplemented.h"
#include "EffekseerRendererDX11.TextureLoader.h"

#include "../../EffekseerRendererCommon/EffekseerRenderer.DXTK.DDSTextureLoader.h"
#include "../../EffekseerRendererCommon/EffekseerRenderer.PngTextureLoader.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererDX11
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
TextureLoader::TextureLoader( Renderer* renderer, ::Effekseer::FileInterface* fileInterface )
	: m_renderer		( renderer )
	, m_fileInterface	( fileInterface )
{
	if( m_fileInterface == NULL )
	{
		m_fileInterface = &m_defaultFileInterface;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
TextureLoader::~TextureLoader()
{

}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void* TextureLoader::Load( const EFK_CHAR* path )
{
	std::auto_ptr<::Effekseer::FileReader> 
		reader( m_fileInterface->OpenRead( path ) );
	
	if( reader.get() != NULL )
	{
		ID3D11ShaderResourceView* texture = NULL;

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

				auto hr = m_renderer->GetDevice()->CreateTexture2D(&TexDesc, &data, &tex);

				if (FAILED(hr))
				{
					delete [] data_texture;
					return (void*)texture;
				}
			
				D3D11_SHADER_RESOURCE_VIEW_DESC desc;
				ZeroMemory(&desc, sizeof(desc));
				desc.Format = TexDesc.Format;
				desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				desc.Texture2D.MostDetailedMip = 0;
				desc.Texture2D.MipLevels = TexDesc.MipLevels;

				hr = m_renderer->GetDevice()->CreateShaderResourceView(tex, &desc, &texture);
				if (FAILED(hr))
				{
					ES_SAFE_RELEASE(texture);
					delete [] data_texture;
					return (void*)texture;
				}

				ES_SAFE_RELEASE(tex);
			}
		}
		else if( data_texture[0] == 'D' &&
			data_texture[1] == 'D' &&
			data_texture[2] == 'S' &&
			data_texture[3] == ' ')
		{
			ID3D11Resource* textureR = NULL;
			EffekseerDirectX::CreateDDSTextureFromMemory(
				m_renderer->GetDevice(),
				(uint8_t*)data_texture,
				size_texture,
				&textureR,
				&texture );

			ES_SAFE_RELEASE(textureR);
		}
		else
		{
			#if __EFFEKSEER_RENDERER_DIRECTXTEX || __EFFEKSEER_RENDERER_DIRECTXTEX__
					::DirectX::ScratchImage img;
					::DirectX::TexMetadata metadata;
			
					::DirectX::LoadFromWICMemory(
						data_texture,
						size_texture,
						0,
						&metadata,
						img );
			
					::DirectX::CreateShaderResourceView(
						m_renderer->GetDevice(),
						img.GetImages(),
						1,
						metadata,
						&texture );
			
					img.Release();
			#else
					D3DX11CreateShaderResourceViewFromMemory(
						m_renderer->GetDevice(), 
						data_texture, 
						size_texture,
						NULL,
						NULL,
						&texture,
						NULL );
			#endif
		}

		delete [] data_texture;
		return (void*)texture;
	}

	return NULL;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void TextureLoader::Unload( void* data )
{
	if( data != NULL )
	{
		ID3D11ShaderResourceView* texture = (ID3D11ShaderResourceView*)data;
		texture->Release();
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