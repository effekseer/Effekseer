
#include "Effekseer.DefaultEffectLoader.h"
#include "../Effekseer.h"
#include <memory>

namespace Effekseer
{

DefaultEffectLoader::DefaultEffectLoader(FileInterfaceRef fileInterface)
	: m_fileInterface(fileInterface)
{
	if (m_fileInterface == nullptr)
	{
		m_fileInterface = MakeRefPtr<DefaultFileInterface>();
	}
}

DefaultEffectLoader::~DefaultEffectLoader()
{
}

bool DefaultEffectLoader::Load(const char16_t* path, void*& data, int32_t& size)
{
	assert(path != nullptr);

	data = nullptr;
	size = 0;

	auto reader = m_fileInterface->OpenRead(path);
	if (reader == nullptr)
		return false;

	size = (int32_t)reader->GetLength();
	data = new uint8_t[size];
	reader->Read(data, size);

	return true;
}

void DefaultEffectLoader::Unload(void* data, int32_t size)
{
	uint8_t* data8 = (uint8_t*)data;
	ES_SAFE_DELETE_ARRAY(data8);
}

} // namespace Effekseer
