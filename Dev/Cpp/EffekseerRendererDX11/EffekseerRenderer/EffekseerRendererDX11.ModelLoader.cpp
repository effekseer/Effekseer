
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererDX11.ModelLoader.h"
#include "EffekseerRendererDX11.Renderer.h"
#include <memory>

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererDX11
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ModelLoader::ModelLoader(ID3D11Device* device, ::Effekseer::FileInterface* fileInterface)
	: device(device)
	, m_fileInterface(fileInterface)
{
	ES_SAFE_ADDREF(device);

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
	ES_SAFE_RELEASE(device);
}

void* ModelLoader::Load(const EFK_CHAR* path)
{
	std::unique_ptr<::Effekseer::FileReader> reader(m_fileInterface->OpenRead(path));
	if (reader.get() == NULL)
		return false;

	if (reader.get() != NULL)
	{
		size_t size_model = reader->GetLength();
		uint8_t* data_model = new uint8_t[size_model];
		reader->Read(data_model, size_model);

		Model* model = (Model*)Load(data_model, size_model);

		delete[] data_model;

		return (void*)model;
	}

	return nullptr;
}

void* ModelLoader::Load(const void* data, int32_t size)
{
	Model* model = new Model((uint8_t*)data, size, device);

	model->ModelCount = Effekseer::Min(Effekseer::Max(model->GetModelCount(), 1), 40);

	model->InternalModels = new Model::InternalModel[model->GetFrameCount()];

	return model;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
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
} // namespace EffekseerRendererDX11
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

#endif // __EFFEKSEER_RENDERER_INTERNAL_LOADER__
