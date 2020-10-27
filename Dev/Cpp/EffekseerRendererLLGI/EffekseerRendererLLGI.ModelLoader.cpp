
#include "EffekseerRendererLLGI.ModelLoader.h"
#include "EffekseerRendererLLGI.Renderer.h"
#include <memory>

namespace EffekseerRendererLLGI
{

ModelLoader::ModelLoader(GraphicsDevice* graphicsDevice, ::Effekseer::FileInterface* fileInterface)
	: graphicsDevice_(graphicsDevice)
	, m_fileInterface(fileInterface)
{
	LLGI::SafeAddRef(graphicsDevice_);

	if (m_fileInterface == NULL)
	{
		m_fileInterface = &m_defaultFileInterface;
	}
}

ModelLoader::~ModelLoader()
{
	LLGI::SafeRelease(graphicsDevice_);
}

void* ModelLoader::Load(const EFK_CHAR* path)
{
	std::unique_ptr<::Effekseer::FileReader> reader(m_fileInterface->OpenRead(path));
	if (reader.get() == NULL)
		return nullptr;

	if (reader.get() != NULL)
	{
		size_t size_model = reader->GetLength();
		uint8_t* data_model = new uint8_t[size_model];
		reader->Read(data_model, size_model);

		Model* model = (Model*)Load(data_model, size_model);

		delete[] data_model;

		return (void*)model;
	}

	return NULL;
}

void* ModelLoader::Load(const void* data, int32_t size)
{
	Model* model = new Model(static_cast<uint8_t*>(const_cast<void*>(data)), size, graphicsDevice_);

	model->ModelCount = Effekseer::Min(Effekseer::Max(model->GetModelCount(), 1), 40);

	model->InternalModels = new Model::InternalModel[model->GetFrameCount()];

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

} // namespace EffekseerRendererLLGI
