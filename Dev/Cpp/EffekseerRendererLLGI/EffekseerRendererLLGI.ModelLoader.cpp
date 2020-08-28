
#include "EffekseerRendererLLGI.ModelLoader.h"
#include "EffekseerRendererLLGI.Renderer.h"
#include "GraphicsDevice.h"
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

		auto* model = new EffekseerRenderer::Model(data_model, size_model, 1, graphicsDevice_->GetGraphicsDevice());

		//Model* model = (Model*)Load(data_model, size_model);

		delete[] data_model;

		return (void*)model;
	}

	return NULL;
}

void* ModelLoader::Load(const void* data, int32_t size)
{
	auto* model = new EffekseerRenderer::Model((uint8_t*)(data), size, 1, graphicsDevice_->GetGraphicsDevice());

	//model->ModelCount = Effekseer::Min(Effekseer::Max(model->GetModelCount(), 1), 40);
	//
	//model->InternalModels = new Model::InternalModel[model->GetFrameCount()];

	return model;
}

void ModelLoader::Unload(void* data)
{
	if (data != NULL)
	{
		EffekseerRenderer::Model* model = (EffekseerRenderer::Model*)data;
		delete model;
	}
}

} // namespace EffekseerRendererLLGI
