#include "ModelLoader.h"

namespace EffekseerRenderer
{

ModelLoader::ModelLoader(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice,
						 ::Effekseer::FileInterfaceRef fileInterface)
	: graphicsDevice_(graphicsDevice)
	, fileInterface_(fileInterface)
{
	if (fileInterface == nullptr)
	{
		fileInterface_ = Effekseer::MakeRefPtr<Effekseer::DefaultFileInterface>();
	}
}

ModelLoader::~ModelLoader()
{
}

::Effekseer::ModelRef ModelLoader::Load(const char16_t* path)
{
	auto reader = fileInterface_->OpenRead(path);
	if (reader == nullptr)
	{
		return nullptr;
	}

	size_t size = reader->GetLength();
	Effekseer::CustomAlignedVector<uint8_t> data;
	data.resize(size);

	reader->Read(data.data(), size);

	auto model = Load(data.data(), (int32_t)size);

	return model;
}

::Effekseer::ModelRef ModelLoader::Load(const void* data, int32_t size)
{
	auto model = ::Effekseer::MakeRefPtr<::Effekseer::Model>((const uint8_t*)data, size);

	return model;
}

void ModelLoader::Unload(::Effekseer::ModelRef data)
{
}

} // namespace EffekseerRenderer
