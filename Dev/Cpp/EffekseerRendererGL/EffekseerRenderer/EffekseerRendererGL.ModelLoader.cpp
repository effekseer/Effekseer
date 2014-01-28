
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <memory>
#include "EffekseerRendererGL.Renderer.h"
#include "EffekseerRendererGL.ModelLoader.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererGL
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ModelLoader::ModelLoader( ::Effekseer::FileInterface* fileInterface )
	: m_fileInterface	( fileInterface )
{
	if( m_fileInterface == NULL )
	{
		m_fileInterface = &m_defaultFileInterface;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ModelLoader::~ModelLoader()
{

}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void* ModelLoader::Load( const EFK_CHAR* path )
{
	std::auto_ptr<Effekseer::FileReader> 
		reader( m_fileInterface->OpenRead( path ) );
	
	if( reader.get() != NULL )
	{
		size_t size_model = reader->GetLength();
		char* data_model = new char[size_model];
		reader->Read( data_model, size_model );

		Effekseer::Model model_main( data_model, size_model );
		
		Model* model = new Model( 
			model_main.GetVertexes(), model_main.GetVertexCount(),
			model_main.GetFaces(), model_main.GetFaceCount());

		delete [] data_model;

		return (void*)model;
	}

	return NULL;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ModelLoader::Unload( void* data )
{
	if( data != NULL )
	{
		Model* model = (Model*)data;
		delete model;
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