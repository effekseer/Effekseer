
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererDX9.ModelLoader.h"
#include "EffekseerRendererDX9.Renderer.h"
#include <memory>

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererDX9
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ModelLoader::ModelLoader(RendererImplemented* renderer, ::Effekseer::FileInterface* fileInterface)
	: renderer_(renderer)
	, m_fileInterface(fileInterface)
{
	ES_SAFE_ADDREF(renderer_);

	if (m_fileInterface == NULL)
	{
		m_fileInterface = &m_defaultFileInterface;
	}

	graphicsDevice_ = new Backend::GraphicsDevice(renderer_->GetDevice());
}

ModelLoader::ModelLoader(LPDIRECT3DDEVICE9 device, ::Effekseer::FileInterface* fileInterface)
	: device_(device)
	, m_fileInterface(fileInterface)
{
	ES_SAFE_ADDREF(device);

	if (m_fileInterface == NULL)
	{
		m_fileInterface = &m_defaultFileInterface;
	}

	graphicsDevice_ = new Backend::GraphicsDevice(device_);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ModelLoader::~ModelLoader()
{
	ES_SAFE_RELEASE(device_);
	ES_SAFE_RELEASE(renderer_);
	ES_SAFE_RELEASE(graphicsDevice_);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Effekseer::Model* ModelLoader::Load(const char16_t* path)
{
	std::unique_ptr<::Effekseer::FileReader> reader(m_fileInterface->OpenRead(path));

	if (reader.get() != nullptr)
	{
		size_t size_model = reader->GetLength();
		uint8_t* data_model = new uint8_t[size_model];
		reader->Read(data_model, size_model);

		auto model = new Effekseer::Model(data_model, (int32_t)size_model);

		// auto model = (Model*)Load(data_model, static_cast<int32_t>(size_model));

		delete[] data_model;

		return model;
	}

	return nullptr;
}

Effekseer::Model* ModelLoader::Load(const void* data, int32_t size)
{
	// get device
	LPDIRECT3DDEVICE9 device = nullptr;
	if (device_ != nullptr)
	{
		device = device_;
	}
	else if (renderer_ != nullptr)
	{
		device = renderer_->GetDevice();
	}
	else
	{
		return nullptr;
	}

	auto model = new Effekseer::Model((uint8_t*)data, size);

	return model;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ModelLoader::Unload(Effekseer::Model* data)
{
	if (data != NULL)
	{
		auto model = (Effekseer::Model*)data;
		delete model;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRendererDX9
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

#endif // __EFFEKSEER_RENDERER_INTERNAL_LOADER__