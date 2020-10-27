
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererGL.ModelLoader.h"
#include "EffekseerRendererGL.Renderer.h"
#include <memory>

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererGL
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ModelLoader::ModelLoader(::Effekseer::FileInterface* fileInterface)
	: m_fileInterface(fileInterface)
{
	if (m_fileInterface == NULL)
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

void* ModelLoader::Load(const EFK_CHAR* path)
{
	std::unique_ptr<Effekseer::FileReader>
		reader(m_fileInterface->OpenRead(path));

	if (reader.get() != NULL)
	{
		size_t size_model = reader->GetLength();
		char* data_model = new char[size_model];
		reader->Read(data_model, size_model);

		Model* model = (Model*)Load(data_model, (int32_t)size_model);

		delete[] data_model;

		return (void*)model;
	}

	return NULL;
}

void* ModelLoader::Load(const void* data, int32_t size)
{
	Model* model = new Model((uint8_t*)data, size);
	return model;
}

void ModelLoader::Unload(void* data)
{
	if (data != NULL)
	{
		Model* model = (Model*)data;
		delete model;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRendererGL
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
