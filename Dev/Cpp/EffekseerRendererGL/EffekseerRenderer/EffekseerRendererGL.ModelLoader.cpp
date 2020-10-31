
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
ModelLoader::ModelLoader(::Effekseer::FileInterface* fileInterface, OpenGLDeviceType deviceType)
	: m_fileInterface(fileInterface)
{
	if (m_fileInterface == nullptr)
	{
		m_fileInterface = &m_defaultFileInterface;
	}

	graphicsDevice_ = new Backend::GraphicsDevice(deviceType);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ModelLoader::~ModelLoader()
{
	ES_SAFE_RELEASE(graphicsDevice_);
}

Effekseer::Model* ModelLoader::Load(const char16_t* path)
{
	std::unique_ptr<Effekseer::FileReader> reader(m_fileInterface->OpenRead(path));

	if (reader.get() != nullptr)
	{
		size_t size_model = reader->GetLength();
		char* data_model = new char[size_model];
		reader->Read(data_model, size_model);

		// Model* model = (Model*)Load(data_model, (int32_t)size_model);

		auto model = new Effekseer::Model((uint8_t*)data_model, (int32_t)size_model);

		delete[] data_model;

		return model;
	}

	return nullptr;
}

Effekseer::Model* ModelLoader::Load(const void* data, int32_t size)
{
	// Model* model = new Model((uint8_t*)data, size);
	auto model = new Effekseer::Model((uint8_t*)data, size);
	return model;
}

void ModelLoader::Unload(Effekseer::Model* data)
{
	if (data != nullptr)
	{
		auto model = (Effekseer::Model*)data;
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
