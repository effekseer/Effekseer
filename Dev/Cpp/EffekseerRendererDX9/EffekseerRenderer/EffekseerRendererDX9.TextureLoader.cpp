
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererDX9.RendererImplemented.h"
#include "EffekseerRendererDX9.TextureLoader.h"

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

#if __EFFEKSEER_RENDERER_DIRECTXTEX || __EFFEKSEER_RENDERER_DIRECTXTEX__
		return NULL;
#else
		D3DXCreateTextureFromFileInMemory( m_renderer->GetDevice(), data_texture, size_texture, &texture );
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