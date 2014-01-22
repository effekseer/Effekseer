#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRenderer.RendererImplemented.h"
#include "EffekseerRenderer.TextureLoader.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
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