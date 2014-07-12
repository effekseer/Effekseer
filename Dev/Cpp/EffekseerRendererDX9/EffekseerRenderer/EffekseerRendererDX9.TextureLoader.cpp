
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererDX9.RendererImplemented.h"
#include "EffekseerRendererDX9.TextureLoader.h"

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
		IDirect3DTexture9* texture = NULL;

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
				HRESULT hr;
				auto width = ::EffekseerRenderer::PngTextureLoader::GetWidth();
				auto height = ::EffekseerRenderer::PngTextureLoader::GetHeight();
				auto mipMapCount = 1;
				hr = m_renderer->GetDevice()->CreateTexture( 
					width,
					height,
					mipMapCount,
					0,
					D3DFMT_A8R8G8B8,
					D3DPOOL_DEFAULT,
					&texture,
					NULL );

				if(FAILED(hr))
				{
					delete [] data_texture;
					return (void*)texture;
				}

				LPDIRECT3DTEXTURE9 tempTexture = NULL;
				hr = m_renderer->GetDevice()->CreateTexture( 
					width,
					height,
					mipMapCount,
					0,
					D3DFMT_A8R8G8B8,
					D3DPOOL_SYSTEMMEM,
					&tempTexture,
					NULL );

				if(FAILED(hr))
				{
					delete [] data_texture;
					return (void*)texture;
				}

				auto srcBits = (uint8_t*)::EffekseerRenderer::PngTextureLoader::GetData().data();
				D3DLOCKED_RECT locked;
				if(SUCCEEDED(tempTexture->LockRect(0,&locked,NULL,0)))
				{
					auto destBits = (uint8_t*)locked.pBits;

					for( auto h = 0; h < height; h++ )
					{
						memcpy( destBits, srcBits, width * 4 );

						// RGB“ü‚ê‘Ö‚¦
						for( auto w = 0; w < width; w++ )
						{
							std::swap( destBits[w * 4 + 0], destBits[w * 4 + 2]);
						}

						destBits += locked.Pitch;
						srcBits += (width * 4);
					}

					tempTexture->UnlockRect( 0 );
				}

				hr = m_renderer->GetDevice()->UpdateTexture( tempTexture, texture );
				ES_SAFE_RELEASE( tempTexture );
			}
		}
		else if( data_texture[0] == 'D' &&
			data_texture[1] == 'D' &&
			data_texture[2] == 'S' &&
			data_texture[3] == ' ')
		{
			EffekseerDirectX::CreateDDSTextureFromMemory(
				m_renderer->GetDevice(),
				(uint8_t*)data_texture,
				size_texture,
				texture );
		}
		else
		{
			#if __EFFEKSEER_RENDERER_DIRECTXTEX || __EFFEKSEER_RENDERER_DIRECTXTEX__
			#else
					D3DXCreateTextureFromFileInMemory( m_renderer->GetDevice(), data_texture, size_texture, &texture );
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
		IDirect3DTexture9* texture = (IDirect3DTexture9*)data;
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