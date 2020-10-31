
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

	if (m_fileInterface == nullptr)
	{
		m_fileInterface = &m_defaultFileInterface;
	}
}

ModelLoader::~ModelLoader()
{
	LLGI::SafeRelease(graphicsDevice_);
}

Effekseer::Model* ModelLoader::Load(const char16_t* path)
{
	std::unique_ptr<::Effekseer::FileReader> reader(m_fileInterface->OpenRead(path));
	if (reader.get() == nullptr)
		return nullptr;

	if (reader.get() != nullptr)
	{
		size_t size_model = reader->GetLength();
		uint8_t* data_model = new uint8_t[size_model];
		reader->Read(data_model, size_model);

		auto* model = new Effekseer::Model(data_model, (int32_t)size_model);

		//Model* model = (Model*)Load(data_model, size_model);

		delete[] data_model;

		return model;
	}

	return nullptr;
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
	if (data != nullptr)
	{
		Effekseer::Model* model = (Effekseer::Model*)data;
		delete model;
	}
}

} // namespace EffekseerRendererLLGI
