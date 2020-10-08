
#include "EffekseerRendererLLGI.ModelLoader.h"
#include "EffekseerRendererLLGI.Renderer.h"
#include "GraphicsDevice.h"
#include <memory>

namespace EffekseerRendererLLGI
{

ModelLoader::ModelLoader(Backend::GraphicsDevice* graphicsDevice, ::Effekseer::FileInterface* fileInterface)
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

Effekseer::Model* ModelLoader::Load(const EFK_CHAR* path)
{
	std::unique_ptr<::Effekseer::FileReader> reader(m_fileInterface->OpenRead(path));
	if (reader.get() == NULL)
		return nullptr;

	if (reader.get() != NULL)
	{
		size_t size_model = reader->GetLength();
		uint8_t* data_model = new uint8_t[size_model];
		reader->Read(data_model, size_model);

		auto* model = new Effekseer::Model(data_model, size_model);

		//Model* model = (Model*)Load(data_model, size_model);

		delete[] data_model;

		return model;
	}

	return NULL;
}

Effekseer::Model* ModelLoader::Load(const void* data, int32_t size)
{
	auto* model = new Effekseer::Model((uint8_t*)(data), size);

	//model->ModelCount = Effekseer::Min(Effekseer::Max(model->GetModelCount(), 1), 40);
	//
	//model->InternalModels = new Model::InternalModel[model->GetFrameCount()];

	return model;
}

void ModelLoader::Unload(Effekseer::Model* data)
{
	if (data != NULL)
	{
		Effekseer::Model* model = (Effekseer::Model*)data;
		delete model;
	}
}

} // namespace EffekseerRendererLLGI
