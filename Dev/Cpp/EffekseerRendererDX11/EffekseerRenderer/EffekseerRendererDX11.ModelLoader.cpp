﻿
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

	graphicsDevice_ = new Backend::GraphicsDevice(device, nullptr);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ModelLoader::~ModelLoader()
{
	ES_SAFE_RELEASE(device);

	ES_SAFE_RELEASE(graphicsDevice_);
}

Effekseer::Model* ModelLoader::Load(const char16_t* path)
{
	std::unique_ptr<::Effekseer::FileReader> reader(m_fileInterface->OpenRead(path));
	if (reader.get() == NULL)
		return false;

	if (reader.get() != NULL)
	{
		size_t size_model = reader->GetLength();
		uint8_t* data_model = new uint8_t[size_model];
		reader->Read(data_model, size_model);

		auto model = Load(data_model, size_model);

		delete[] data_model;

		return model;
	}

	return nullptr;
}

Effekseer::Model* ModelLoader::Load(const void* data, int32_t size)
{
	return new Effekseer::Model((uint8_t*)data, size);
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
} // namespace EffekseerRendererDX11
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

#endif // __EFFEKSEER_RENDERER_INTERNAL_LOADER__
